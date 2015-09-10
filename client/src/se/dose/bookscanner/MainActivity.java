package se.dose.bookscanner;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;

import android.support.v7.app.ActionBarActivity;
import android.support.v4.app.Fragment;
import android.text.method.ScrollingMovementMethod;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.PictureCallback;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends ActionBarActivity {

    private static int CAMERA_FACE_BACK = 1;
    private static int CAMERA_FACE_FRONT = 2;
    
    private Camera mCamera = null;
    private int mCameraFace = 0;
    private CameraPreview cameraPreview;
    private NetworkListener networkListener;
    private int orientation = 0;
	private Socket serverSocket;
	private boolean autoFocusEnabled;
	private boolean justFocus = false;
    
    //Camera.CameraInfo mCameraInfo = new Camera.CameraInfo();
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (savedInstanceState == null) {
            getSupportFragmentManager().beginTransaction()
                    .add(R.id.container, new PlaceholderFragment())
                    .commit();
        }
    	if (!detect_camera())
    	{
    		debug("Could not find camera!");
    		finish();
    		return;
    	}
    	openBackCamera();
    	if(mCamera == null)
    	{
    		debug("No camera!");
    		return;
    	}
    }
    
    @Override
    protected void onPause()
    {
    	super.onPause();
		if(mCamera != null)
			mCamera.release();
		networkListener.stop = true;
		networkListener.cancel(true);
    }
     
    @Override
    protected void onResume()
    {
    	super.onResume();
    	TextView t = (TextView)findViewById(R.id.debugText);
    	t.setMovementMethod(ScrollingMovementMethod.getInstance());
    	cameraPreview = new CameraPreview(this, mCamera);
    	if(cameraPreview == null)
    	{
    		debug("No camera preview!");
    		return;
    	}
    	FrameLayout p = (FrameLayout) findViewById(R.id.camera_preview);
    	if(p == null)
    	{
    		debug("No Preview Frame found!");
    		return;
    	}
    	p.addView(cameraPreview);
    	networkListener = new NetworkListener();
    	networkListener.execute();
    }
    
    private Camera.AutoFocusCallback autoFocusDone = new AutoFocusCallback()
    {
		@Override
		public void onAutoFocus(boolean success, Camera camera) {
			
			// Did we just want to focus, or did we also want to snap a picture afterwards?
			if(!justFocus)
				mCamera.takePicture(null, null, pictureCallback);
			justFocus = false;
	        		
		}
    };
    
    private PictureCallback pictureCallback = new PictureCallback()
    {

        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
			debug("Sharing picture...");
            try {
				debug("Size of picture: " + data.length + " bytes.");
				OutputStream os = serverSocket.getOutputStream();

				// Command (one byte) + size (four bytes) + image data
				byte buffer[] = new byte[4 + 1 + data.length];
				ByteBuffer bb = ByteBuffer.wrap(buffer);
				
				// Write command to buffer
				bb.put(Commands.SEND_IMAGE);
				
				// Write size of JPEG image to buffer
				bb.putInt(data.length);

				// Write actual image to buffer
				bb.put(data);

				debug(String.format("First bytes of buffer. Command: %X, size: %X %X %X %X, data: %X %X %X %X %X", buffer[0], buffer[1], buffer[2],
						buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10]));
		
				// Write buffer to socket
				debug(String.format("Size: ", bb.limit()));
				os.write(buffer);
				
				// Restart preview so that we can take more pictures
				mCamera.startPreview();
				
		    } catch (Exception e) {
                debug("Exception when sending image on socket!" + e.toString());
            }
        }
    };
    
    /**
     * Checks whether the device has a camera
     * @return
     * 		true	- Device has camera
     * 		false	- Device has no camera
     * 
     */
    private boolean detect_camera()
    {
    	
		if(getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA))
			return true;
		else
			return false;
    	
    }

    /**
     * Returns from the picture snapping
     */
    protected void onActivityResult(int requestCode, int resultCode, Intent i)
    {
    	/*
    	if(requestCode == REQUEST_IMAGE && resultCode == RESULT_OK)
    	{
    		Bundle extras = i.getExtras();
    		Bitmap b = (Bitmap) extras.get("data");
    		ImageView v = (ImageView) findViewById(R.id.previewImage);
    		v.setImageBitmap(b);
    	    debug("Snapped!");
    	}
    	*/
    }
    /**
     * Called when the snap! button is clicked
     * @param view
     */
    public void rotateButtonClicked(View view)
    {
    	if(orientation >= 270)
    		orientation = 0;
    	else
    		orientation += 90;
    	debug("Setting orientation to: " + orientation);
    	mCamera.setDisplayOrientation(orientation);
    }

    public void snapButtonClicked(View view)
    {
    	mCamera.autoFocus(autoFocusDone);
    	
    }
        
    
    public void switchCameraButtonClicked(View view)
    {	
    	if(mCameraFace == CAMERA_FACE_BACK)
    		openFrontCamera();
    	else
    		openBackCamera();
    	cameraPreview = new CameraPreview(this, mCamera);
    	
    	// Update the CameraPreview to use the new camera
    	FrameLayout p = (FrameLayout) findViewById(R.id.camera_preview);
    	p.removeAllViews();
    	p.addView(cameraPreview);
    }

    
    private void openFrontCamera()
    {
    	// First, make sure we have more than one camera!
    	try{
    		if(Camera.getNumberOfCameras() < 2)
    		{
    			debug("Device has only one camera!");
    			return;
    		}
    	} catch(Exception e)
    	{
    		debug("Could not get the number of cameras!");
    		return;
    	}

    	for(int i = 0; i < Camera.getNumberOfCameras(); i++)
    	{
    		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        	Camera.getCameraInfo(i, cameraInfo);
    		if(cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT)
    		{
    			try {
    				if(mCamera != null)
    					mCamera.release();
    				mCamera = Camera.open(i);
    				mCameraFace = CAMERA_FACE_FRONT;
    				debug("Front camera opened!");
    			} catch (Exception e)
    			{
    				debug("Could not open front camera!");
    			}
    		}
    	}
    }

    private void openBackCamera()
    {
    	// First, make sure we have more than one camera!
    	try{
    		if(Camera.getNumberOfCameras() < 2)
    		{
    			debug("Device has only one camera!");
    			return;
    		}
    	} catch(Exception e)
    	{
    		debug("Could not get the number of cameras!");
    		return;
    	}

    	for(int i = 0; i < Camera.getNumberOfCameras(); i++)
    	{
    		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        	Camera.getCameraInfo(i, cameraInfo);
    		if(cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_BACK)
    		{
    			try {
    				if(mCamera != null)
    					mCamera.release();
    				mCamera = Camera.open(i);
    				mCameraFace = CAMERA_FACE_BACK;
    				Camera.Parameters p = mCamera.getParameters();
    				//p.setFlashMode(Camera.Parameters.FLASH_MODE_ON);
    				p.setPreviewFormat(ImageFormat.JPEG);
    				debug("Preview format: " + p.getPreviewFormat());
    				/*
    				p.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
    				maxZoom = p.getMaxZoom();
    				debug("Max zoom: " + maxZoom);
    				SeekBar zb = (SeekBar) findViewById(R.id.zoomBar);
    				zb.setMax(maxZoom);
    				zb.setOnSeekBarChangeListener(zoomChangeListener);
    				debug("Focus mode: " + p.getFocusMode());
    				*/
    				//p.setExposureCompensation(p.getMaxExposureCompensation());
    				mCamera.setParameters(p);
    				debug("Back camera opened!");
    			} catch (Exception e)
    			{
    				debug("Could not open back camera!");
    			}
    		}
    	}
    }
    
  	
    SeekBar.OnSeekBarChangeListener zoomChangeListener = new SeekBar.OnSeekBarChangeListener() {
		
		@Override
		public void onStopTrackingTouch(SeekBar seekBar) {
			// TODO Auto-generated method stub
			
		}
		
		@Override
		public void onStartTrackingTouch(SeekBar seekBar) {
			// TODO Auto-generated method stub
			
		}
		
		@Override
		public void onProgressChanged(SeekBar seekBar, int progress,
				boolean fromUser) {
			Camera.Parameters p = mCamera.getParameters();
			p.setZoom(progress);
			mCamera.setParameters(p);
			debug("zoom changed to: " + progress);
		}
	};

    
    /**
     * Toasts out a debug text
     * @param text
     */
    private void debug(String text)
    {
    	TextView debugText;
    	if(findViewById(R.id.debugText) != null)
    	{
    		debugText = (TextView) findViewById(R.id.debugText);
    		debugText.append("\n" + text);
    		if(debugText.getLineCount() > 4)
    			debugText.setScrollY(debugText.getLineCount() * debugText.getLineHeight() - debugText.getLineHeight() * 4);
    	} else
    	{
    		Toast.makeText(getApplicationContext(), text, 
    				Toast.LENGTH_SHORT).show();
    	}
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {

        public PlaceholderFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.fragment_main, container, false);
            return rootView;
        }
    }
    
    void sendMaxZoom()
    {
    	try {
			OutputStream os = serverSocket.getOutputStream();
			
			// One command byte + one four-byte size int + one four-byte max zoom int
			byte buffer[] = new byte[1 + 4 + 4];
			ByteBuffer bb = ByteBuffer.wrap(buffer);
			
			// Command
			bb.put(Commands.MAX_ZOOM);
			
			// Size of package
			bb.putInt(4);
			
			// Max zoom
			bb.putInt(mCamera.getParameters().getMaxZoom());
			os.write(buffer);
			os.flush();
			
		} catch (IOException e) {
			debug("Could not send max zoom!" + e);
		}
    	
    }
    
    private class NetworkListener extends AsyncTask<Void, String, Void>
    {
    	private byte[] inBuffer;
    	public boolean stop;

    	@Override
    	protected Void doInBackground(Void... args) {
    		try {

    			InetAddress address = InetAddress.getByName("192.168.1.117");
    			serverSocket = new Socket(address, 10000);
    			publishProgress("We have a socket!") ;
        		InputStream is = serverSocket.getInputStream();
        		
        		while(!stop)
    			{
    				// Read the command
    				inBuffer = new byte[1];
    				if(is.read(inBuffer, 0, 1) < 0)
    				{
    					publishProgress("Server disappeared!");
    					break;
    				}
    				
    				publishProgress(String.format("Command: %d", inBuffer[0]));
    				
    				switch(inBuffer[0])
    				{
    				
    				case Commands.SNAP:
    					if(autoFocusEnabled)
    						mCamera.autoFocus(autoFocusDone);
    					else
    						mCamera.takePicture(null, null, pictureCallback);
    					break;
    					
    				case Commands.AUTOFOCUS_ENABLE:
    					autoFocusEnabled = true;
    					break;
    					
    				case Commands.AUTOFOCUS_DISABLE:
    					autoFocusEnabled = false;
    					break;
    					
    				case Commands.FOCUS:
    					justFocus = true;
    					mCamera.autoFocus(autoFocusDone);
    					break;
    					
    				case Commands.ASK_FOR_MAX_ZOOM:
    					sendMaxZoom();
    					break;
    					
    				case Commands.SET_ZOOM:
    					
    					// Size of package (one four-byte int) + zoom (one four-byte int)
    					byte buffer[] = new byte[4 + 4];
    					is.read(buffer);
    					ByteBuffer bb = ByteBuffer.wrap(buffer);

    					// The size of the zoom package is always four bytes (i.e., one int)
    					int zoomSize = bb.getInt();
    					if(zoomSize != 4)
    					{
    						publishProgress("Zoom package size error!" + zoomSize);
    						String s = String.format("Data: %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X",
    								buffer[0], buffer[1], buffer[2], buffer[3],
    								buffer[4], buffer[5], buffer[6], buffer[7]);
    						publishProgress(s);
    						serverSocket.close();
    						break;
    					}
    					
    					Camera.Parameters p = mCamera.getParameters();
    					int zoom = bb.getInt();
    					publishProgress("Zoom set to: " + zoom);
    					p.setZoom(zoom);
    					mCamera.setParameters(p);
    					break;
    					
    				case Commands.DIE:
    					publishProgress("Client killed us!");
    					stop = true;
    					break;
    					
    				}
    				
    			} // while(!stop)
    			serverSocket.close();
    			serverSocket = null;

    			//finish();
    		} catch (Exception e) {
    			publishProgress("ERROR!");
    			publishProgress(e.toString());
    			if(serverSocket != null)
    				try {
    					serverSocket.close();
    					serverSocket = null;
    				} catch (IOException e1) {
    			}
    			//finish();
    		}
    		return null;
    	}
    	
    	protected void onProgressUpdate(String... strings)
    	{
    		// Send a message back to the calling activity
    		debug(strings[0]);
    	}
    	protected void onPostExecute(Void arg) {
    	}
    } // class CanvasListener



}


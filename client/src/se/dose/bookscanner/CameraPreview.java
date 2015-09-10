package se.dose.bookscanner;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;

import android.content.Context;
import android.graphics.Canvas;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.widget.Toast;

public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback, Camera.PreviewCallback {

	private SurfaceHolder holder;
	private Camera camera;
	
	public CameraPreview(Context context, Camera camera) {
		super(context);
		this.camera = camera;
		holder = getHolder();
		holder.addCallback(this);
		holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
	}
	
	public CameraPreview(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public CameraPreview(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		try {
			camera.setPreviewDisplay(holder);
			camera.startPreview();
			camera.setPreviewCallback(this);
		} catch (Exception e) {
			debug("surfaceCreated() error!");
		}
	}

	private int noChanges = 0;
	@Override
	public void draw(Canvas c)
	{
		super.dispatchDraw(c);
		noChanges += 1;
		//if(noChanges % 100 == 0)
			debug("Changes: " + noChanges);
	}
	
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		
		if(holder.getSurface() == null)
			return;
		
		try {
			camera.stopPreview();
		} catch (Exception e) {
			debug("stopPreview() error!");
		}
		
		try {
			camera.setPreviewDisplay(holder);
			camera.startPreview();
		} catch (Exception e) {
			debug("startPreview() error!");
		}
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub

	}
	
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		debug("Size of preview: " + data.length + " bytes.");
		/*
		OutputStream os;
		try {
			os = serverSocket.getOutputStream();
	
			// Command (one byte) + size (four bytes) + image data
			byte buffer[] = new byte[4 + 1 + data.length];
			ByteBuffer bb = ByteBuffer.wrap(buffer);
			
			// Write command to buffer
			bb.put(Commands.SEND_IMAGE);
			
			// Write size of JPEG image to buffer
			bb.putInt(data.length);
	
			// Write actual image to buffer
			bb.put(data);
	
			// Write buffer to socket
			debug(String.format("Size: ", bb.limit()));
			os.write(buffer);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		*/
	}

   private void debug(String text)
    {
    	Toast.makeText(getContext(), text, 
    		   Toast.LENGTH_SHORT).show();
    }
 
}

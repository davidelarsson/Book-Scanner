package se.dose.bookscanner;

public class Commands
{
	// From client to server
	public final static byte SNAP = 42;		// No size + data
	public final static byte DIE = 43;		// Not used!
	public final static byte MAX_ZOOM = 77;
	
	
	// From server to client
	public final static byte SEND_IMAGE = 47;		// No size + data
	public final static byte AUTOFOCUS_ENABLE = 52; // No size + data
	public final static byte AUTOFOCUS_DISABLE = 53; // No size + data
	public final static byte FOCUS = 62;			// No size + data
	public final static byte SET_ZOOM = 72;
	public final static byte ASK_FOR_MAX_ZOOM = 82; // No size + data
	
	
}

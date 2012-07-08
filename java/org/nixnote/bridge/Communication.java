package org.nixnote.bridge;

import org.apache.thrift.TException;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.transport.THttpClient;
import org.apache.thrift.transport.TTransportException;

import com.evernote.edam.error.EDAMSystemException;
import com.evernote.edam.error.EDAMUserException;
import com.evernote.edam.notestore.NoteStore;
import com.evernote.edam.type.User;
import com.evernote.edam.userstore.AuthenticationResult;
import com.evernote.edam.userstore.Constants;
import com.evernote.edam.userstore.UserStore;
import com.evernote.edam.userstore.UserStore.Client;

public class Communication {

	private boolean keepRunning;
	private boolean isConnected;
	private Client userStore;
	private com.evernote.edam.notestore.NoteStore.Client noteStore;
	private User user;
	private AuthenticationResult authResult;
	private String authToken;
    private static final String consumerKey = "baumgarte";
    private static final String consumerSecret = "eb8b5740e17cb55f";
    
    private static final String userAgent = "Evernote/EDAMDemo (Java) " + 
            Constants.EDAM_VERSION_MAJOR + "." + 
            Constants.EDAM_VERSION_MINOR;
	
	private static final String evernoteHost = "sandbox.evernote.com";
	private static final String userStoreUrl = "https://" + evernoteHost + "/edam/user";
	private static final String noteStoreUrlBase = "https://" + evernoteHost + "/edam/note/";
	
	public void setKeepRunning(boolean v) {
		keepRunning = v;
	}
	
	public boolean isConnected() {
		return isConnected;
	}
	
	public MessageBlock connect(String username, String password) {
		MessageBlock returnBlock = new MessageBlock();
	    THttpClient userStoreTrans;
		try {
			userStoreTrans = new THttpClient(userStoreUrl);
			userStoreTrans.setCustomHeader("User-Agent", "Nixnote2/Test");
			TBinaryProtocol userStoreProt = new TBinaryProtocol(userStoreTrans);
			userStore = new UserStore.Client(userStoreProt, userStoreProt);
	    
			// Check that we can talk to the server
			boolean versionOk = userStore.checkVersion("Nixnote2",
			com.evernote.edam.userstore.Constants.EDAM_VERSION_MAJOR,
			com.evernote.edam.userstore.Constants.EDAM_VERSION_MINOR);
			if (!versionOk) {
				returnBlock.setErrorMessage("Incomatible EDAM client protocol version");
				return returnBlock;
			}
			
			// Authenticate
			authResult = userStore.authenticate(username, password, consumerKey, consumerSecret);
			authToken = authResult.getAuthenticationToken();
			
		    User user = authResult.getUser();
		    String shardId = user.getShardId();
		    
		    System.out.println("Successfully authenticated as " + user.getUsername());
		    
		    // Set up the NoteStore client 
		    String noteStoreUrl = noteStoreUrlBase + shardId;
		    THttpClient noteStoreTrans = new THttpClient(noteStoreUrl);
		    noteStoreTrans.setCustomHeader("User-Agent", userAgent);
		    TBinaryProtocol noteStoreProt = new TBinaryProtocol(noteStoreTrans);
		    noteStore = new NoteStore.Client(noteStoreProt, noteStoreProt);

			
		} catch (TTransportException e) {
			e.printStackTrace();
			returnBlock.setErrorMessage(e.getMessage());
			return returnBlock;
		} catch (TException e) {
			e.printStackTrace();
			returnBlock.setErrorMessage(e.getMessage());
			return returnBlock;
		} catch (EDAMUserException e) {
			e.printStackTrace();
			returnBlock.setErrorMessage(e.getMessage());
			return returnBlock;
		} catch (EDAMSystemException e) {
			e.printStackTrace();
			returnBlock.setErrorMessage(e.getMessage());
			return returnBlock;
		}
		System.out.println("Connected");
		return returnBlock;
	}
}

package org.nixnote.bridge;

public class MessageBlock {
	private boolean error;
	
	public MessageBlock() {
		error = false;
		errorMessage = "";
	}
	public void setError(boolean v) {
		error = v;
	}
	public boolean isError() {
		return error;
	}
	
	
	private String errorMessage;
	public String getErrorMessage() {
		return errorMessage;
	}
	public void setErrorMessage(String e) {
		if (!e.trim().equals("")) 
			setError(true);
		else
			setError(false);
		errorMessage = e;
	}

}

package cx.fbn.encrypt;



public class Crypto {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		EnCrypt encrypt = new EnCrypt();
		
		if (args.length != 4) 
			System.exit(16);
		
		String function = args[0];
		String password = args[1];
		String keylen = args[2];
		String text = args[3];
		
		String result = "";
		if (function.equalsIgnoreCase("encrypt-rc2"))
			result = encrypt.encryptRC2(text, password, Integer.parseInt(keylen));		
		else if (function.equalsIgnoreCase("decrypt-rc2"))
			result = encrypt.decryptRC2(text, password, Integer.parseInt(keylen));
		else
			System.exit(14);
		
		if (result.equals(""))
			System.exit(4);
		
	    System.out.print(result);  
	}
}

package net.strus.example;
import net.strus.api.*;

public class Status
{
	public static void main( String []args) {
		String config = "path=storage";
		// Create the storage client:
		StrusContext ctx = new StrusContext();
		StorageClient storage = ctx.createStorageClient( config);
		// Query the number of documents inserted:
		long nofDocuments = storage.nofDocumentsInserted();
		// Output:
		System.out.println( "Number of documents inserted: " + nofDocuments);
	}
} 


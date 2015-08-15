package net.strus.example;
import net.strus.api.*;

public class Status
{
	public static void main( String []args) {
		String config = "path=storage";
		StrusContext ctx = new StrusContext();
		StorageClient storage = ctx.createStorageClient( config);
		long nofDocuments = storage.nofDocumentsInserted();
		System.out.println( "Number of documents inserted: " + nofDocuments);
	}
} 


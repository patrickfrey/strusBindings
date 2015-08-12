package net.strus.example;
import net.strus.api.*;

public class Status
{
	public static void main( String []args) {
		String config = "localhost:7181";
		if (args.length > 0)
		{
			config = args[ 0];
		}
		StrusContext ctx = new StrusContext( config);
		StorageClient storage = ctx.createStorageClient("");
		long nofDocuments = storage.nofDocumentsInserted();
		System.out.println( "Number of documents inserted: " + nofDocuments);
	}
} 


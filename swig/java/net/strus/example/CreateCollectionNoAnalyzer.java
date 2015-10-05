package net.strus.example;
import net.strus.api.*;
import java.io.*;
import java.util.List;

public class CreateCollectionNoAnalyzer
{
	// Insert a document defined by its parts passed as arguments:
	public static void insertDoc( StorageClient storage, String docid, String[] searchIndex, String[] forwardIndex, String title)
	{
		// Create the document to insert:
		Document doc = new Document();
		int pos = 0;
		// Define the search index (inverted index) for the document:
		for (String item : searchIndex)
		{
			++pos;
			if (item.length() > 0)
			{
				doc.addSearchIndexTerm( "word", item, pos);
			}
		}
		// Define the forward index (items for summary) for the document:
		pos = 0;
		for (String item : forwardIndex)
		{
			++pos;
			if (item != null)
			{
				doc.addForwardIndexTerm( "orig", item, pos);
			}
		}
		// Define the document title:
		doc.setAttribute( "title", title);
		// Prepare the document for insert (the insert will be finalized with storage.flush()):
		storage.insertDocument( docid, doc);
	}

	public static void main( String []args) {
		String config = "path=storage; metadata=doclen UINT16";
		if (args.length > 0)
		{
			config = args[ 0];
		}
		// Create context and storage:
		Context ctx = new Context();
		try
		{
			ctx.destroyStorage( config);
			//... destroy storage if it already exists
		}
		catch (Exception e)
		{
			//... ignore if storage did not exist
		}
		ctx.createStorage( config);
		StorageClient storage = ctx.createStorageClient( config);

		// Define the test documents (The terms of the document are already normalized here):
		String[] doc_A_searchIndex = {"tokyo","is","a","citi","that","is","complet","differ","than","what","you","would","expect","as","european","citizen"};
		String[] doc_A_forwardIndex = {"Tokyo","is","a","city","that","is","completely","different","than","what","you","would","expect","as","European","citizen."};
		String doc_A_title = "One day in Tokyo";

		String[] doc_B_searchIndex = {"new", "york", "is", "a", "citi", "with", "dimens", "you", "can", "t", "imagine"};
		String[] doc_B_forwardIndex = {"New", "York", "is", "a", "city", "with", "dimensions", "you", "can't", null, "imagine"};
		String doc_B_title = "A visit in New York";

		String[] doc_C_searchIndex = {"when","i","first","visit","germani","it","was","still","split","into","two","part"};
		String[] doc_C_forwardIndex = {"When","I","first","visited","germany","it","was","still","splitted","into","two","parts."};
		String doc_C_title = "A journey through Germany";

		// Insert the test documents:
		try
		{
			insertDoc( storage, "A", doc_A_searchIndex, doc_A_forwardIndex, doc_A_title);
			insertDoc( storage, "B", doc_B_searchIndex, doc_B_forwardIndex, doc_B_title);
			insertDoc( storage, "C", doc_C_searchIndex, doc_C_forwardIndex, doc_C_title);

			// Without this commit the documents wont be inserted:
			storage.flush();
		}
		catch (Exception e)
		{
			System.err.println( "Failed to insert documents: " + e.getMessage());
			return;
		}
		System.out.println( "done");
	}
} 


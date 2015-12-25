package net.strus.example;
import net.strus.api.*;
import java.io.*;
import java.util.List;

public class CreateCollection
{
	// Read a file and return its contents
	static String readFile( String path) throws IOException {
		try (BufferedReader br = new BufferedReader(new FileReader(path)))
		{
			StringBuilder sb = new StringBuilder();
			String line = br.readLine();
			
			while (line != null) {
				sb.append(line);
				sb.append(System.lineSeparator());
				line = br.readLine();
			}
			return sb.toString();
		}
	}

	public static void main( String []args) {
		// Get the storage configuration string from arguments or a default:
		String config = "path=storage; metadata=doclen UINT16";
		if (args.length > 0)
		{
			config = args[ 0];
		}
		// Create context, storage and analyzer:
		Context ctx = new Context();
		try
		{
			ctx.destroyStorage( config);
			//... delete the storage files if they already exists
		}
		catch (Exception e)
		{
			//... ignore if storage did not exist
		}
		// Create a new storage:
		System.out.println("Create storage with config '" + config + "'");
		ctx.createStorage( config);
		// Get a client for the new created storage:
		StorageClient storage = ctx.createStorageClient( config);

		// Define the document analyzer to use:
		DocumentAnalyzer analyzer = ctx.createDocumentAnalyzer();
		{
			// Define the tokenizers:
			Tokenizer word_tokenizer = new Tokenizer( "word");	//... tokenizer for word boundaries
			Tokenizer split_tokenizer = new Tokenizer( "split");	//... tokenizer to split by whitespaces
			Tokenizer content_tokenizer = new Tokenizer( "content");//... tokenizer doing nothing
	
			// Create the normalizer for the search terms:
			NormalizerVector stem_normalizer = new NormalizerVector(3);
			stem_normalizer.set( 0, new Normalizer( "stem", "en"));	//... snowball stemming
			stem_normalizer.set( 1, new Normalizer( "lc"));		//... lowercase
			stem_normalizer.set( 2, new Normalizer( "convdia", "en"));//... convert diachritical characters
	
			// Create the normalizer for the forward index (summary) and the title:
			NormalizerVector orig_normalizer = new NormalizerVector(1);
			orig_normalizer.set( 0, new Normalizer( "orig"));	

			// Define the features and attributes to store:
			analyzer.addSearchIndexFeature( "word", "/doc/text()", word_tokenizer, stem_normalizer); 
			analyzer.addForwardIndexFeature( "orig", "/doc/text()", split_tokenizer, orig_normalizer);
			analyzer.defineAttribute( "title", "/doc/title()", content_tokenizer, orig_normalizer);
		}
		// Read input files, analyze and insert them:
		try
		{
			// Get the files to analyze and insert:
			StorageTransaction transaction = storage.createTransaction();
			String datadir = "./data/";
			File folder = new File( datadir);
			File[] listOfFiles = folder.listFiles();

			for (File file : listOfFiles) {
				if (file.isFile())
				{
					String filename = file.getName();
					if (filename.endsWith( ".xml"))
					{
						// Analyze and insert the document:
						String docid = filename.substring( 0, filename.length()-4);
						Document doc = analyzer.analyze( readFile( datadir + filename));
						transaction.insertDocument( docid, doc);
					}
				}
			}
			// Without this the documents wont be inserted:
			transaction.commit();
		}
		catch (Exception e)
		{
			System.err.println( "Failed to read all input files: " + e.getMessage());
			return;
		}
		System.out.println( "done");
	}
} 


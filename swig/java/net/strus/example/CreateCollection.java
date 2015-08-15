package net.strus.example;
import net.strus.api.*;
import java.io.*;
import java.util.List;

public class CreateCollection
{
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
		String config = "path=storage";
		if (args.length > 0)
		{
			config = args[ 0];
		}
		// Create context, storage and analyzer:
		StrusContext ctx = new StrusContext();
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
		DocumentAnalyzer analyzer = ctx.createDocumentAnalyzer();

		// Define the analyzer:
		Tokenizer word_tokenizer = new Tokenizer( "word");
		Tokenizer split_tokenizer = new Tokenizer( "split");
		Tokenizer content_tokenizer = new Tokenizer( "content");

		NormalizerVector stem_normalizer = new NormalizerVector(3);
		stem_normalizer.set( 0, new Normalizer( "stem", "en"));
		stem_normalizer.set( 1, new Normalizer( "lc"));
		stem_normalizer.set( 2, new Normalizer( "convdia", "en"));
		NormalizerVector orig_normalizer = new NormalizerVector(1);
		orig_normalizer.set( 0, new Normalizer( "orig"));

		analyzer.addSearchIndexFeature( "word", "/doc/text()", word_tokenizer, stem_normalizer); 
		analyzer.addForwardIndexFeature( "orig", "/doc/text()", split_tokenizer, orig_normalizer);
		analyzer.defineAttribute( "title", "/doc/title()", content_tokenizer, orig_normalizer);

		// Read input files, analyze and insert them:
		try
		{
			String datadir = "./data/";
			File folder = new File( datadir);
			File[] listOfFiles = folder.listFiles();

			for (File file : listOfFiles) {
				if (file.isFile())
				{
					String filename = file.getName();
					if (filename.endsWith( ".xml"))
					{
						String docid = filename.substring( 0, filename.length()-4);
						Document doc = analyzer.analyze( readFile( datadir + filename));
						storage.insertDocument( docid, doc);
					}
				}
			}
			// Without this commit the documents wont be inserted:
			storage.flush();
		}
		catch (Exception e)
		{
			System.err.println( "Failed to read all input files: " + e.getMessage());
			return;
		}
		System.out.println( "done");
	}
} 


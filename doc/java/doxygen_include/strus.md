strus Java bindings	 {#mainpage}
===================

This interface documentation has been generated from the base C++ interface with injection of some
additional stuff like namespaces and helper classes, that are specific for the Java interface.
Unfortunately it contains still some type qualifiers pointing to it's C++ origin. 
Please don't blame it for that. I'll find other solutions. Suggestions are welcome. 

The strus Java bindings provide a Java interface for accessing the 
retrieval storage, indexing documents and queries and evaluating queries.

The entry point of a strus application with Java is the 
context (@ref net::strus::api::Context) object.
It is the root object from which all other objects are created.
All objects of the strus Java API are in the namespace <i>net.strus.api</i>.
It can be constructed either as proxy, that redirects all method calls to an RpcServer
or it can be constructed as instance running in the Java/JNI environment.

\section CreateContextFile This example shows the creation of the root object Context that accesses the storage directly.
\code
String storageConfig = "path=storage; metadata=doclen UINT16";
net.strus.api.Context ctx = new net.strus.api.Context();
net.strus.api.StorageClient storage = ctx.createStorageClient( config);
\endcode

\section CreateContextRpc This example shows the creation of the root object Context as RPC proxy.
\code
String rpcServer = "localhost:7181";
Context ctx = new Context( rpcServer);
net.strus.api.StorageClient storage = ctx.createStorageClient("");
\endcode

\section CreateCollectionNoAnalyzer Create a collection of document (without using the strus analyzer)
In the Java universe there exist a lot of alternatives to analyze or index a document. This example
accommodates this and shows how to insert a test collection defined without using the strus analyzer.
The terms are assumed to come from "somewhere".
\code
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
\endcode

\section CreateCollection Create a collection of documents (with strus analyzer)
Now we show an example doing the same as the previous one, but reading its documents to process
from files and processing them with the strus document analyzer.
\code
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
						storage.insertDocument( docid, doc);
					}
				}
			}
			// Without this the documents wont be inserted:
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
\endcode

\section QueryNoAnalyzer Retrieve a ranklist with a simple query consisting of some terms (without analyzer)
The query evaluation scheme used for ranking the results is BM25. The policy that decides what to return is
defined by a selection expression that matches documents that contain all of the query terms.
So we search for documents that contain all query terms and rank them with BM25:

\code
package net.strus.example;
import net.strus.api.*;
import java.io.*;
import java.util.List;

public class QueryNoAnalyzer
{
	// Evaluate a query defined by some terms with a query evaluation scheme passed:
	public static RankVector evaluateQuery( StorageClient storage, QueryEval queryEval, String[] terms)
	{
		// Create a query to instantiate for the query evaluation scheme passed:
		Query query = queryEval.createQuery( storage);

		// Iterate on the terms and create a single term feature for each term and collect
		// all terms to create a selection expression out of them:
		QueryExpression selectexpr = new QueryExpression();
		for (String term : terms) {
			// We push the query terms on the stack and create a query feature 'seek' 
			// for each of it:
			QueryExpression expr = new QueryExpression();
			expr.pushTerm( "word", term);
			selectexpr.pushTerm( "word", term);

			// We assign the features created to the set named 'seek' because they are 
			// referenced with this name in the query evaluation:
			query.defineFeature( "seek", expr, 1.0);
		}
		// Create a selection feature 'select' that matches documents that contain all query terms.
		// We assign the feature created to the set named 'select' because this is the
		// name of the set defined as selection feature in the query evaluation configuration
		// (QueryEval.addSelectionFeature):
		selectexpr.pushExpression( "contains", terms.length);
		query.defineFeature( "select", selectexpr);

		// Define the maximum number of best result (ranks) to return:
		query.setMaxNofRanks( 20);
		// Define the index of the first rank (for implementing scrolling: 0 for the first, 
		// 20 for the 2nd, 40 for the 3rd page, etc.):
		query.setMinRank( 0);
		return query.evaluate();
	}

	public static QueryEval createQueryEval( Context ctx) {
		QueryEval queryEval = ctx.createQueryEval();

		// Here we define what query features decide, what is ranked for the result:
		queryEval.addSelectionFeature( "select");

		// Here we define how we rank a document selected. We use the 'BM25' weighting scheme:
		WeightingConfig weighting = new WeightingConfig();
		weighting.defineParameter( "k1", 0.75);		//... k1 paramater of BM25
		weighting.defineParameter( "b", 2.1);		//... b paramater of BM25
		weighting.defineParameter( "avgdoclen", 1000);	//... average document length passed as parameter
		weighting.defineFeature( "match", "seek");	//... we search for features of the set we call 'seek'
		queryEval.addWeightingFunction( 1.0, "BM25", weighting);
	
		// Now we define what attributes of the documents are returned and how they are build.
		// The functions that extract stuff from documents for presentation are called summarizers.
		// First we add a summarizer that extracts us the title of the document:
		SummarizerConfig sum_title = new SummarizerConfig();
		sum_title.defineParameter( "name", "title");
		queryEval.addSummarizer( "title", "attribute", sum_title);
	
		// Then we add a summarizer that collects the sections that enclose the best matches 
		// in a ranked document:
		SummarizerConfig sum_match = new SummarizerConfig();
		sum_match.defineParameter( "type", "orig");
		sum_match.defineParameter( "nof", 4);
		sum_match.defineParameter( "len", 60);
		sum_match.defineFeature( "match", "seek");
		queryEval.addSummarizer( "summary", "matchphrase", sum_match);

		// Now we are done:
		return queryEval;
	}

	public static void main( String[] args) {
		if (args.length == 0)
		{
			args = new String[1];
			args[0] = "citi";
		}
		// Create the storage client (the storage is assumed to be created with 'CreateCollectionNoAnalyzer.java'):
		Context ctx = new Context();
		String config = "path=storage";
		StorageClient storage = ctx.createStorageClient( config);
		// Create the query evaluation scheme:
		QueryEval queryEval = createQueryEval( ctx);

		// Evaluate the query:
		RankVector results = evaluateQuery( storage, queryEval, args);

		// We iterate on the results returned and we print them:
		System.out.println( "Number of results: " + results.size());
		int pos = 0;
		for (Rank result : results)
		{
			++pos;
			System.out.println( "rank " + pos + ": " + result.docno() + " " + result.weight() + ":");
			RankAttributeVector attributes = result.attributes();
			for (RankAttribute attribute : attributes)
			{
				System.out.println( "\t" + attribute.name() + ": " + attribute.value());
			}
		}
		System.out.println( "done");
	}
} 
\endcode


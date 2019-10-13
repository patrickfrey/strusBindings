require "testUtils"

config = {
	data = {
		workdir     = "/srv/strus/build/tests/request_luasim",
		configdir   = "/etc/strus/build/tests/request_luasim/config",
		resources   = "/usr/share/strus/tests/request_luasim/strus"
	},
	transactions = {
		max_idle_time  = 600,
		nof_per_sec = 60
	},
	extensions = {
		directory = "/usr/local/lib/strus/modules",
		modules = { 
			"modstrus_analyzer_pattern",
			"modstrus_storage_vector_std"
		}
	},
	http = {
		script = "/strus"
	},
	service = {
		worker_threads = 1,
		name = "test",
		api = "http",
		ip = "0.0.0.0",
		port = 8080,
	},
	logging = {
		level = "debug",
		file = {
			name = "./strusWebService.{service.name}.log",
			append = true
		}
	},
	debug = {
		log_requests = true,
		struct_depth = 4,
		request_file = "./requests.{service.name}.log",
		protocol = {
			pretty_print = true,
			quit_enabled = true,
			debug_enabled = true
		}
	}
}

checkExpected( to_json( config), "@configToJson.exp", "configToJson.res" )



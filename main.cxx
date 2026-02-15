#include <impl/includes.h>

int main( ) {
	SetConsoleTitleA( encrypt( "popstar-external" ) );
	SetUnhandledExceptionFilter( exception::exception_filter );



	if (!mem::find_driver( ) )
		logging::print(encrypt("Failed to find drvier"));

		return std::getchar( );

	std::wstring target = L"FortniteClient-Win64-Shipping.exe";
	int pid = 0;
	while (!pid) {
		pid = mem::find_process(target.c_str());
		logging::print(encrypt("CHP"));

		mem::process_id = pid;

	}
	cr3_a = mem::fetch_cr3();
	logging::print(encrypt("cr3"));

	if (!cr3_a) {
		logging::print(encrypt("Failed to resolve DTB."));
		return false;
	}

	virtualaddy = mem::find_image();
	logging::print(encrypt("base"));

	if (!virtualaddy) {
		logging::print( encrypt( "Failed to attach process." ) );
		return std::getchar( );
	}

	if ( !g_drawing->hijack( ) ) {
		logging::print( encrypt( "Failed to find overlay." ) );
		return std::getchar( );
	}

	if ( !g_drawing->setup( ) ) {
		logging::print( encrypt( "Failed to setup drawing." ) );
		return std::getchar( );
	}

	g_world->init( );
	g_config->get_available_configs( );

	drawing::render( );

	logging::print(encrypt("LOADED"));

	return std::getchar( );
}
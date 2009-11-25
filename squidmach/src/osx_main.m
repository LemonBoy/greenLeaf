#include <Foundation/NSAutoreleasePool.h>

int main(int argc, char *argv[])
{
	NSApplicationLoad();
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	int retVal = osx_main(argc, argv);
	[pool release];
	return retVal;
}

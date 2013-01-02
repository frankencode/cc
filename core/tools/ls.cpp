#include <ftl/stdio>
#include <ftl/process>

int main(int argc, char **argv)
{
	using namespace ftl;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			Ref<Glob, Owner> glob = Glob::open(argv[i]);
			for (String path; glob->read(&path);)
				output()->writeLine(path);
		}
	}
	else {
		Ref<Dir, Owner> dir = Dir::open(".");
		for (String name; dir->read(&name);)
			output()->writeLine(name);
	}

	return 0;
}
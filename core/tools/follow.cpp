#include <ftl/PrintDebug.hpp>
#include <ftl/Thread.hpp>
#include <ftl/File.hpp>

int main(int argc, char **argv)
{
	using namespace ftl;

	if (argc != 2) return 1;
	Ref<File> file = File::open(argv[1]);
	off_t nw = 0;
	Ref<ByteArray> buf = ByteArray::create(FTL_DEFAULT_BUF_CAPA);
	while (true) {
		if (File::status(file->path())->size() > nw) {
			while (true) {
				int nr = file->readAvail(buf);
				if (nr == 0) break;
				nw += nr;
				rawOutput()->write(buf->data(), nr);
			}
		}
		Thread::sleep(1);
	}

	return 0;
}

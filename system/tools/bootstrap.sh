#! /bin/sh -x
mkdir -p .ftlcore-0.3.0-x86_64-linux-gnu
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/AbnfCoreSyntax.o ../ftl/AbnfCoreSyntax.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/syntax.o ../ftl/syntax.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/File.o ../ftl/File.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/MemoryMapping.o ../ftl/MemoryMapping.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/new.o ../ftl/new.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/FormatSpecifier.o ../ftl/FormatSpecifier.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Date.o ../ftl/Date.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxState.o ../ftl/SyntaxState.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Dir.o ../ftl/Dir.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/GccToolChain.o ../ftl/GccToolChain.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/LineSource.o ../ftl/LineSource.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/AbnfSyntax.o ../ftl/AbnfSyntax.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/LinkInfo.o ../ftl/LinkInfo.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/UriSyntax.o ../ftl/UriSyntax.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/IntegerLiteral.o ../ftl/IntegerLiteral.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Token.o ../ftl/Token.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ByteEncoder.o ../ftl/ByteEncoder.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/DependencyCache.o ../ftl/DependencyCache.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/User.o ../ftl/User.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/BitEncoder.o ../ftl/BitEncoder.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/FileStatus.o ../ftl/FileStatus.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxDebugFactory.o ../ftl/SyntaxDebugFactory.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/FileLock.o ../ftl/FileLock.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Condition.o ../ftl/Condition.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Group.o ../ftl/Group.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SystemStream.o ../ftl/SystemStream.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SocketAddressEntry.o ../ftl/SocketAddressEntry.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxDefinition.o ../ftl/SyntaxDefinition.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxDebugger.o ../ftl/SyntaxDebugger.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Buffer.o ../ftl/Buffer.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Glob.o ../ftl/Glob.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ProcessFactory.o ../ftl/ProcessFactory.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Pipe.o ../ftl/Pipe.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/InetAddressSyntax.o ../ftl/InetAddressSyntax.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ThreadExitEvent.o ../ftl/ThreadExitEvent.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Event.o ../ftl/Event.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Mutex.o ../ftl/Mutex.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ByteDecoder.o ../ftl/ByteDecoder.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/GlobalCoreMutex.o ../ftl/GlobalCoreMutex.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/LineSink.o ../ftl/LineSink.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/BitDecoder.o ../ftl/BitDecoder.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ExitEvent.o ../ftl/ExitEvent.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/BuildPlan.o ../ftl/BuildPlan.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Format.o ../ftl/Format.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SocketAddress.o ../ftl/SocketAddress.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/NetworkInterface.o ../ftl/NetworkInterface.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/AbnfCompiler.o ../ftl/AbnfCompiler.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Wire.o ../ftl/Wire.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Json.o ../ftl/Json.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Md5.o ../ftl/Md5.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Pattern.o ../ftl/Pattern.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ByteArray.o ../ftl/ByteArray.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/String.o ../ftl/String.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Exception.o ../ftl/Exception.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ProcessStatus.o ../ftl/ProcessStatus.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Random.o ../ftl/Random.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/FloatLiteral.o ../ftl/FloatLiteral.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Time.o ../ftl/Time.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Semaphore.o ../ftl/Semaphore.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Base64.o ../ftl/Base64.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/HuffmanCodec.o ../ftl/HuffmanCodec.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Config.o ../ftl/Config.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Thread.o ../ftl/Thread.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SignalSet.o ../ftl/SignalSet.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Memory.o ../ftl/Memory.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Process.o ../ftl/Process.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/debug.o ../ftl/debug.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/StreamSocket.o ../ftl/StreamSocket.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/ThreadFactory.o ../ftl/ThreadFactory.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/strings.o ../ftl/strings.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Character.o ../ftl/Character.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Crc32.o ../ftl/Crc32.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Uri.o ../ftl/Uri.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/Stream.o ../ftl/Stream.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/StandardStreams.o ../ftl/StandardStreams.cpp
g++ -std=c++0x -fpic -Wall -pthread -c -o .ftlcore-0.3.0-x86_64-linux-gnu/SignalManager.o ../ftl/SignalManager.cpp
g++ -shared -pthread -Wl,-soname,libftlcore.so.0 -o libftlcore.so.0.3.0 .ftlcore-0.3.0-x86_64-linux-gnu/AbnfCoreSyntax.o .ftlcore-0.3.0-x86_64-linux-gnu/syntax.o .ftlcore-0.3.0-x86_64-linux-gnu/File.o .ftlcore-0.3.0-x86_64-linux-gnu/MemoryMapping.o .ftlcore-0.3.0-x86_64-linux-gnu/new.o .ftlcore-0.3.0-x86_64-linux-gnu/FormatSpecifier.o .ftlcore-0.3.0-x86_64-linux-gnu/Date.o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxState.o .ftlcore-0.3.0-x86_64-linux-gnu/Dir.o .ftlcore-0.3.0-x86_64-linux-gnu/GccToolChain.o .ftlcore-0.3.0-x86_64-linux-gnu/LineSource.o .ftlcore-0.3.0-x86_64-linux-gnu/AbnfSyntax.o .ftlcore-0.3.0-x86_64-linux-gnu/LinkInfo.o .ftlcore-0.3.0-x86_64-linux-gnu/UriSyntax.o .ftlcore-0.3.0-x86_64-linux-gnu/IntegerLiteral.o .ftlcore-0.3.0-x86_64-linux-gnu/Token.o .ftlcore-0.3.0-x86_64-linux-gnu/ByteEncoder.o .ftlcore-0.3.0-x86_64-linux-gnu/DependencyCache.o .ftlcore-0.3.0-x86_64-linux-gnu/User.o .ftlcore-0.3.0-x86_64-linux-gnu/BitEncoder.o .ftlcore-0.3.0-x86_64-linux-gnu/FileStatus.o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxDebugFactory.o .ftlcore-0.3.0-x86_64-linux-gnu/FileLock.o .ftlcore-0.3.0-x86_64-linux-gnu/Condition.o .ftlcore-0.3.0-x86_64-linux-gnu/Group.o .ftlcore-0.3.0-x86_64-linux-gnu/SystemStream.o .ftlcore-0.3.0-x86_64-linux-gnu/SocketAddressEntry.o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxDefinition.o .ftlcore-0.3.0-x86_64-linux-gnu/SyntaxDebugger.o .ftlcore-0.3.0-x86_64-linux-gnu/Buffer.o .ftlcore-0.3.0-x86_64-linux-gnu/Glob.o .ftlcore-0.3.0-x86_64-linux-gnu/ProcessFactory.o .ftlcore-0.3.0-x86_64-linux-gnu/Pipe.o .ftlcore-0.3.0-x86_64-linux-gnu/InetAddressSyntax.o .ftlcore-0.3.0-x86_64-linux-gnu/ThreadExitEvent.o .ftlcore-0.3.0-x86_64-linux-gnu/Event.o .ftlcore-0.3.0-x86_64-linux-gnu/Mutex.o .ftlcore-0.3.0-x86_64-linux-gnu/ByteDecoder.o .ftlcore-0.3.0-x86_64-linux-gnu/GlobalCoreMutex.o .ftlcore-0.3.0-x86_64-linux-gnu/LineSink.o .ftlcore-0.3.0-x86_64-linux-gnu/BitDecoder.o .ftlcore-0.3.0-x86_64-linux-gnu/ExitEvent.o .ftlcore-0.3.0-x86_64-linux-gnu/BuildPlan.o .ftlcore-0.3.0-x86_64-linux-gnu/Format.o .ftlcore-0.3.0-x86_64-linux-gnu/SocketAddress.o .ftlcore-0.3.0-x86_64-linux-gnu/NetworkInterface.o .ftlcore-0.3.0-x86_64-linux-gnu/AbnfCompiler.o .ftlcore-0.3.0-x86_64-linux-gnu/Wire.o .ftlcore-0.3.0-x86_64-linux-gnu/Json.o .ftlcore-0.3.0-x86_64-linux-gnu/Md5.o .ftlcore-0.3.0-x86_64-linux-gnu/Pattern.o .ftlcore-0.3.0-x86_64-linux-gnu/ByteArray.o .ftlcore-0.3.0-x86_64-linux-gnu/String.o .ftlcore-0.3.0-x86_64-linux-gnu/Exception.o .ftlcore-0.3.0-x86_64-linux-gnu/ProcessStatus.o .ftlcore-0.3.0-x86_64-linux-gnu/Random.o .ftlcore-0.3.0-x86_64-linux-gnu/FloatLiteral.o .ftlcore-0.3.0-x86_64-linux-gnu/Time.o .ftlcore-0.3.0-x86_64-linux-gnu/Semaphore.o .ftlcore-0.3.0-x86_64-linux-gnu/Base64.o .ftlcore-0.3.0-x86_64-linux-gnu/HuffmanCodec.o .ftlcore-0.3.0-x86_64-linux-gnu/Config.o .ftlcore-0.3.0-x86_64-linux-gnu/Thread.o .ftlcore-0.3.0-x86_64-linux-gnu/SignalSet.o .ftlcore-0.3.0-x86_64-linux-gnu/Memory.o .ftlcore-0.3.0-x86_64-linux-gnu/Process.o .ftlcore-0.3.0-x86_64-linux-gnu/debug.o .ftlcore-0.3.0-x86_64-linux-gnu/StreamSocket.o .ftlcore-0.3.0-x86_64-linux-gnu/ThreadFactory.o .ftlcore-0.3.0-x86_64-linux-gnu/strings.o .ftlcore-0.3.0-x86_64-linux-gnu/Character.o .ftlcore-0.3.0-x86_64-linux-gnu/Crc32.o .ftlcore-0.3.0-x86_64-linux-gnu/Uri.o .ftlcore-0.3.0-x86_64-linux-gnu/Stream.o .ftlcore-0.3.0-x86_64-linux-gnu/StandardStreams.o .ftlcore-0.3.0-x86_64-linux-gnu/SignalManager.o -lrt -lutil -ldl
ln -sf libftlcore.so.0.3.0 libftlcore.so.0.3
ln -sf libftlcore.so.0.3.0 libftlcore.so.0
ln -sf libftlcore.so.0.3.0 libftlcore.so
mkdir -p .mach-x86_64-linux-gnu
g++ -std=c++0x -Wall -pthread -I.. -c -o .mach-x86_64-linux-gnu/mach.o mach.cpp
g++ -pthread -o mach .mach-x86_64-linux-gnu/mach.o -L. -lftlcore -Wl,--enable-new-dtags,-rpath=/home/frank/src/FTL/system/tools

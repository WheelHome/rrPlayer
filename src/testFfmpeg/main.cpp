#include <QCoreApplication>
#include <iostream>
extern "C"{
    #include <libavcodec/avcodec.h>
};
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::cout << "Test FFmpeg" << std::endl;
    std::cout << avcodec_configuration() << std::endl;
    return 0;
}

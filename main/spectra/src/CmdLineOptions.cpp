#include "CmdLineOptions.h"

namespace Viewer {

CmdLineOptions::CmdLineOptions() : m_args()
{
}

void CmdLineOptions::parse(int argc, char *argv[]) {

    m_args.resize(argc);
    for (int i=0; i<argc; ++i) {
        m_args[i] = argv[i];
    }

    cmdline_parser(argc, argv, &m_info);
}

int CmdLineOptions::getPort() {
    return m_info.port_arg;
}

QString CmdLineOptions::getHost()
{
    return QString(m_info.host_arg);
}

bool CmdLineOptions::disableTrueTypeFonts() const
{
    return m_info.disable_truetype_flag;
}

} // namespace Viewer

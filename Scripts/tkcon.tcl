package require tkcon
set tkcon::PRIV(showOnStartup) 0
set tkcon::PRIV(root)          .console
set tkcon::PRIV(protocol)      {tkcon hide}
set tkcon::OPT(exec)           ""
tkcon::Init
tkcon title "SpecTcl"
tkcon show

package require history 8.5;	# 8.6 history relies on apply command.
package require tkcon 3.0
set tkcon::PRIV(showOnStartup) 0
set tkcon::PRIV(root)          .console
set tkcon::PRIV(protocol)      {tkcon hide}
set tkcon::OPT(exec)           ""
tkcon::Init
tkcon title "SpecTcl"
tkcon show

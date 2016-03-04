
package require json::write

Direct_Url /spectcl/shmem  SpecTcl_Memory

proc SpecTcl_Memory/key {} {
  set SpecTcl_Memory/key "application/json"

  set key [shmemkey]

  return [::SpecTcl::_returnObject OK [json::write string $key] ]
}

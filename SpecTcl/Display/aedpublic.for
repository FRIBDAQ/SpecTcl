C
C  aedpublic.for -- This module defines the public entry points
C                    for the AED bindings to Xamine.
C                    it should be included by fortran clients to these 
C		     bindings.
C
      LOGICAL AEDGETONE         ! OBSOLETE 
      LOGICAL AEDFREEONE        ! OBSOLETE
      LOGICAL AEDENTER
      LOGICAL AEDTEXT           ! UNIMPLEMENTED.
      LOGICAL AEDDELETE
      LOGICAL AEDLOCATE         ! UNIMPLEMENTED.
      INTEGER*8 AEDSTARTSEARCH
      LOGICAL AEDNEXT1

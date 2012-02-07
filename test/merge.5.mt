## -*- shell-script -*-

TOOL="mddl"
CMDLINE="merge ${srcdir}/merge.a.mddl ${srcdir}/merge.d.mddl"

## STDIN
 
## STDOUT
stdout=$(mktemp)
cat "${srcdir}/merge.a.mddl" > "${stdout}"

## merge.5.mt ends here

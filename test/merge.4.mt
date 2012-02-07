## -*- shell-script -*-

TOOL="mddl"
CMDLINE="merge ${srcdir}/merge.d.mddl ${srcdir}/merge.a.mddl"

## STDIN
 
## STDOUT
stdout=$(mktemp)
cat "${srcdir}/merge.a.mddl" > "${stdout}"

## merge.4.mt ends here

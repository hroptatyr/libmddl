## -*- shell-script -*-

TOOL="mddl"
CMDLINE="merge ${srcdir}/merge.a.mddl ${srcdir}/merge.a.mddl"

## STDIN
 
## STDOUT
stdout=$(mktemp)
cat "${srcdir}/merge.a.mddl" > "${stdout}"

## merge.1.mt ends here

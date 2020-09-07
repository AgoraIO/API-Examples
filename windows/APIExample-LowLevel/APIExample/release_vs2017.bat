pushd Release
pushd Language

del *.exp
del *.pdb
del *.lib
del *.iobj
del *.ipdb

popd Language

del *.pdb
del *.log
del *.ipdb
del *.iobj

IF NOT EXIST vcruntime140.dll (
  copy C:\Windows\SysWOW64\VCRuntime140.dll  Release
)

popd Release
pause
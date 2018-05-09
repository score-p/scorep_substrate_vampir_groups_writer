# Vampir groups writer

This is a small tool, which can write Vampir group files. Currently only the python tracing is supported.

The plugin looks for a single `:` in a region name. Once one is found it looks for a single `.` . The name till this dot (usually the name of a module) will be used as group maker.

## Build
Simply use cmake:

```
mkdir build
cd build
cmake ..
make
```

## Usage

Please add `vampir_groups_writer` to the `SCOREP_SUBSTRATE_PLUGINS` environment variable. E.g.:
```
export SCOREP_SUBSTRATE_PLUGINS="vampir_groups_writer" 
```

The `libscorep_substrate_vampir_groups_writer.so` should be in the `LD_LIBRARY_PATH`

## If anything fails

Please create a Issue on Github. 
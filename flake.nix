{
    description = "Garbage Collector Dev";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
	};

    outputs = { self, ... } @ inputs: 
    let
        pkgs = import inputs.nixpkgs { inherit system; };
        ROOT = let 
            p = builtins.getEnv "PWD";
        in if p != "" then p else self;
        name = "Garbage Collector Dev";
        system = "x86_64-linux";
    in {
        devShells."${system}".default = pkgs.mkShell {
            inherit name ROOT;

            buildInputs = with pkgs; [
                gcc
                gdb
                cmake
            ];

            shellHook = ''
                alias "compile"="pushd ${ROOT}; make all; popd"
                alias "run"="compile; ${ROOT}/bin/mutator"
                alias "debug"="compile; gdb ${ROOT}/bin/mutator -x ${ROOT}/gdb.conf"
                alias "debugc"="compile; gdb ${ROOT}/bin/mutator ${ROOT}/core -x ${ROOT}/gdb.conf"
            '';

        };
    };
}

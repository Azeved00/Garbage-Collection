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

            NIX_SHELL_NAME=name;
            buildInputs = with pkgs; [
                gcc
                gdb
                cmake
            ];

            shellHook = ''
                echo -ne "\033]0;${name}\007"

                alias "compile"="pushd ${ROOT} '''&'''& make all '''&'''& popd"
                echo "${ROOT}"
            '';

        };
    };
}

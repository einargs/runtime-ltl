let
   hostPkgs = import <nixpkgs> {};
   # Generated through:
   # nix-shell -p nix-prefetch-git --run "nix-prefetch-git  https://github.com/nixos/nixpkgs-channels.git --rev {rev} > nixpkgs-version.json"
   pinnedVersion = hostPkgs.lib.importJSON ./nixpkgs-version.json;

   pinnedPkgs = hostPkgs.fetchFromGitHub {
     owner = "NixOS";
     repo = "nixpkgs-channels";
     inherit (pinnedVersion) rev sha256;
   };
 in import pinnedPkgs

{ pkgs ? import (fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/archive/nixos-24.11.tar.gz";
  }) {
    config = {
      allowUnfree = true;
    };
  }
}:

pkgs.mkShell {
  buildInputs = with pkgs; [
    libgcc
    clang-tools
    libGL
    pkg-config
    xorg.libXi
    xorg.libXcursor
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXxf86vm
    xorg.libX11.dev
    xorg.libXft
    xorg.libX11
    alsa-lib.dev
    alsa-lib
    wayland
    libxkbcommon
    go  # nixpkgs 24.11 ships go 1.23.3
    delve
    gdlv
    lua
    clang
    gcc
    bear
  ];

  # this is needed for delve to work with cgo
  # see: https://wiki.nixos.org/wiki/Go#Using_cgo_on_NixOS
  hardeningDisable = [ "fortify" ];

  shellHook = with pkgs; ''
    export LD_LIBRARY_PATH=${pkgs.xorg.libX11}/lib:${pkgs.wayland}/lib:${pkgs.lib.getLib pkgs.libGL}/lib:${pkgs.lib.getLib pkgs.libGL}/lib:$LD_LIBRARY_PATH
  '';
}


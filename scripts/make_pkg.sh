#!/bin/bash
# Crea l'installer macOS Tonante-<versione>-macOS.pkg dai plugin già compilati.
# Uso: scripts/make_pkg.sh [versione] [cartella_artefatti] [cartella_output]
set -euo pipefail
VERSION="${1:-1.0.0}"
ART="${2:-build/Tonante_artefacts/Release}"
OUT="${3:-dist}"
HERE="$(cd "$(dirname "$0")" && pwd)"
WORK="$(mktemp -d)"
mkdir -p "$WORK/au/Library/Audio/Plug-Ins/Components" "$WORK/vst3/Library/Audio/Plug-Ins/VST3" "$WORK/scripts" "$WORK/res" "$OUT"

cp -R "$ART/AU/Tonante.component" "$WORK/au/Library/Audio/Plug-Ins/Components/"
cp -R "$ART/VST3/Tonante.vst3"     "$WORK/vst3/Library/Audio/Plug-Ins/VST3/"

# firma ad-hoc (necessaria su Apple Silicon)
codesign --force --deep -s - "$WORK/au/Library/Audio/Plug-Ins/Components/Tonante.component"
codesign --force --deep -s - "$WORK/vst3/Library/Audio/Plug-Ins/VST3/Tonante.vst3"

# bundle NON rilocabili: installa sempre in /Library/Audio/Plug-Ins
for b in au vst3; do
    pkgbuild --analyze --root "$WORK/$b" "$WORK/$b.plist"
    plutil -replace 0.BundleIsRelocatable -bool NO "$WORK/$b.plist"
done

# dopo l'installazione: forza macOS/Logic a ri-scansionare gli Audio Unit
cat > "$WORK/scripts/postinstall" <<'EOS'
#!/bin/bash
killall -9 AudioComponentRegistrar 2>/dev/null || true
exit 0
EOS
chmod +x "$WORK/scripts/postinstall"

pkgbuild --root "$WORK/au" --component-plist "$WORK/au.plist" --identifier com.tunetalks.tonante.au \
         --version "$VERSION" --install-location / --scripts "$WORK/scripts" "$WORK/Tonante-AU.pkg"
pkgbuild --root "$WORK/vst3" --component-plist "$WORK/vst3.plist" --identifier com.tunetalks.tonante.vst3 \
         --version "$VERSION" --install-location / "$WORK/Tonante-VST3.pkg"

cp "$HERE/pkg/welcome.html" "$WORK/res/"
sed "s/@VERSION@/$VERSION/g" "$HERE/pkg/distribution.xml" > "$WORK/distribution.xml"

productbuild --distribution "$WORK/distribution.xml" --package-path "$WORK" --resources "$WORK/res" \
             "$OUT/Tonante-$VERSION-macOS.pkg"
rm -rf "$WORK"
echo "Creato: $OUT/Tonante-$VERSION-macOS.pkg"

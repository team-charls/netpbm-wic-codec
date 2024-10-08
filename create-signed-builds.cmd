# Build signed x86
msbuild -t:clean -p:Configuration=Release -p:Platform=x86
msbuild -t:restore -p:Configuration=Release -p:Platform=x86
msbuild -t:build -p:Configuration=Release -p:Platform=x86 -p:SignOutput=true -p:CertificateThumbprint=%1 -p:TimestampUrl=%2

# Build signed x64
msbuild -t:clean -p:Configuration=Release -p:Platform=x64
msbuild -t:restore -p:Configuration=Release -p:Platform=x64
msbuild -t:build -p:Configuration=Release -p:Platform=x64 -p:SignOutput=true -p:CertificateThumbprint=%1 -p:TimestampUrl=%2

# Build signed ARM64
msbuild -t:clean -p:Configuration=Release -p:Platform=ARM64
msbuild -t:restore -p:Configuration=Release -p:Platform=ARM64
msbuild -t:build -p:Configuration=Release -p:Platform=ARM64 -p:SignOutput=true -p:CertificateThumbprint=%1 -p:TimestampUrl=%2


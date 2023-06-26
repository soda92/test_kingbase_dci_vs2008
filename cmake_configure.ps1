function vs2008env {
    Push-Location "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin"
    cmd /c "vcvars32.bat & set" |
    ForEach-Object {
        if ($_ -match "=") {
            $v = $_.split("=", 2); Set-Item -Force -Path "ENV:\$($v[0])" -Value "$($v[1])"
        }
    }
    Pop-Location
    Write-Host "Visual Studio 2008 x86 Command Prompt variables set." -ForegroundColor Green
}

vs2008env
cmake -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release
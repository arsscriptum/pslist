<#
#̷\   ⼕龱ᗪ㠪⼕闩丂ㄒ龱尺 ᗪ㠪ᐯ㠪㇄龱尸爪㠪𝓝ㄒ
#̷\   🇵​​​​​🇴​​​​​🇼​​​​​🇪​​​​​🇷​​​​​🇸​​​​​🇭​​​​​🇪​​​​​🇱​​​​​🇱​​​​​ 🇸​​​​​🇨​​​​​🇷​​​​​🇮​​​​​🇵​​​​​🇹​​​​​ 🇧​​​​​🇾​​​​​ 🇨​​​​​🇴​​​​​🇩​​​​​🇪​​​​​🇨​​​​​🇦​​​​​🇸​​​​​🇹​​​​​🇴​​​​​🇷​​​​​@🇮​​​​​🇨​​​​​🇱​​​​​🇴​​​​​🇺​​​​​🇩​​​​​.🇨​​​​​🇴​​​​​🇲​​​​​
#>

[CmdletBinding(SupportsShouldProcess)]
param (
    [Parameter(Mandatory=$false,ValueFromPipeline=$true, 
        HelpMessage="Root Path of all the projects, the parent path common to most") ]
      [string]$DevelopmentRoot
    )

$Name = $script:MyInvocation.MyCommand.Name
$i = $Name.IndexOf('.')
$Script:CurrentScript = $Name.SubString(0, $i)
$Script:CurrPath=Split-Path $script:MyInvocation.MyCommand.Path

If( $PSBoundParameters.ContainsKey('DevelopmentRoot') -eq $True ){
    $Script:DevelopmentRoot = $DevelopmentRoot
}else{
    $Script:DevelopmentRoot = "$Env:Development"
}
Write-Host "Development Root `t" -NoNewLine -f DarkYellow;  Write-Host "$Script:DevelopmentRoot" -f Gray 
$a = Read-Host -p "Development Root is $Script:DevelopmentRoot. OK ? (y/n)"
if($a -notlike "y") {return}
#===============================================================================
# Script Variables
#===============================================================================
$Script:CurrentRunningScript           = $Script:CurrentScript
$Script:Time                           = Get-Date
$Script:Date                           = $Time.GetDateTimeFormats()[19]
$Script:DejaRootPath                   = Join-Path $Script:DevelopmentRoot "dejainsight"
$Script:DejaLibsPath                   = Join-Path $Script:DejaRootPath "lib"
$Script:DejaIncsPath                   = Join-Path $Script:DejaRootPath "include"
$Script:BuildAutomation                = Join-Path $Script:DevelopmentRoot "automation.build.bat"


Write-Host "===============================================================================" -f DarkRed
Write-Host "CONFIGURATION of DEVELOPMENT ENVIRONMENT for BUILD AUTOMATION " -f DarkYellow;
Write-Host "===============================================================================" -f DarkRed    
Write-Host "Current Path     `t" -NoNewLine -f DarkYellow ; Write-Host "$Script:CurrPath" -f Gray 
Write-Host "Current Script   `t" -NoNewLine -f DarkYellow;  Write-Host "$Script:CurrentScript" -f Gray 

Write-Host "Development Root `t" -NoNewLine -f DarkYellow;  Write-Host "$Script:DevelopmentRoot" -f Gray 
Write-Host "Build Automation `t" -NoNewLine -f DarkYellow;  Write-Host "$Script:BuildAutomation" -f Gray 
Write-Host "Deja Root Path   `t" -NoNewLine -f DarkYellow;  Write-Host "$Script:DejaRootPath" -f Gray 
Write-Host "Deja Libs Path   `t" -NoNewLine -f DarkYellow;  Write-Host "$Script:DejaLibsPath" -f Gray 
Write-Host "Deja Incs Path   `t" -NoNewLine -f DarkYellow;  Write-Host "$Script:DejaIncsPath" -f Gray 


Write-Host "===============================================================================" -f DarkRed
Write-Host "SETTING REGISTRY ENTRIES" -f DarkYellow;
Write-Host "===============================================================================" -f DarkRed   

$Script:RegPathAuto="$Env:OrganizationHKCU\development\build-automation"
$Script:RegPathDeja="$Env:OrganizationHKCU\development\dejainsight"
New-Item $Script:RegPathAuto -Force | Out-Null
New-Item $Script:RegPathDeja -Force | Out-Null
Write-Host "[REGISTRY] " -f DarkRed -NonewLine
Write-Host " build-automation" -f DarkYellow
if((Get-Item -Path $Script:RegPathAuto -ErrorAction ignore) -eq $null){
    Write-Host " (o) " -f DarkRed -NoNewLine
    (New-Item -Path $Script:RegPathAuto -Force).Name
}

Write-Host " (o) " -f DarkRed -NoNewLine ; (New-ItemProperty -Path $Script:RegPathAuto -Name "DevelopmentRoot" -Value $Script:DevelopmentRoot -Force).PSPath
Write-Host " (o) " -f DarkRed -NoNewLine ; (New-ItemProperty -Path $Script:RegPathAuto -Name "BuildAutomation" -Value $Script:BuildAutomation -Force).PSPath   
Write-Host " (o) " -f DarkRed -NoNewLine ; (New-ItemProperty -Path $Script:RegPathDeja -Name "SdkDirectory" -Value $Script:DejaRootPath -Force).PSPath   
Write-Host " (o) " -f DarkRed -NoNewLine ; (New-ItemProperty -Path $Script:RegPathDeja -Name "Sdk_Incs" -Value $Script:DejaIncsPath -Force).PSPath 
Write-Host " (o) " -f DarkRed -NoNewLine ; (New-ItemProperty -Path $Script:RegPathDeja -Name "Sdk_Libs" -Value $Script:DejaLibsPath -Force).PSPath

function SetSystemValue{
    [CmdletBinding(SupportsShouldProcess)]
    Param
    (
        [Parameter(Mandatory=$true, Position=0)]
        [string]$Name,
        [Parameter(Mandatory=$true, Position=1)]
        [string]$Value,
        [switch]$CheckPath
    )
    if($CheckPath){
        if(-not(Test-Path $Value)){
            Write-Host ">>> WARNING <<<" -f DarkRed -NoNewLine ; 
            Write-Host " $Value DOESNT EXISTS!" -f DarkYellow ; 
            $a = Read-Host -p "continue ? (y/n)"
            if($a -notlike "y") {exit}
        }
    }
    Write-Host "Setting $Name --> $Value [User]"  -ForegroundColor Yellow
    [System.Environment]::SetEnvironmentVariable($Name,$Value,[System.EnvironmentVariableTarget]::User)
}

Write-Host "===============================================================================" -f DarkRed
Write-Host "SETTING ENVIRONMENT ENTRIES" -f DarkYellow;
Write-Host "===============================================================================" -f DarkRed   

SetSystemValue 'DejaToolsRootDirectory' $Script:DejaRootPath -CheckPath
SetSystemValue 'DejaToolsSdkIncludes' $Script:DejaIncsPath -CheckPath
SetSystemValue 'DejaToolsSdkLibs' $Script:DejaLibsPath -CheckPath
SetSystemValue 'DisableRegistryUse' 'false'
SetSystemValue 'AutomationScriptsRoot' $Script:BuildAutomation -CheckPath
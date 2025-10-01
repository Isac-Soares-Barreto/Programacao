#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/dir.h>

// Classe utilitária para gerenciar caminhos da aplicação
class PathUtils
{
public:
    // Obter diretório do executável
    static wxString GetExecutableDir()
    {
        wxString exePath = wxStandardPaths::Get().GetExecutablePath();
        wxFileName fn(exePath);
        return fn.GetPath() + wxFileName::GetPathSeparator();
    }

    // Obter diretório base do projeto (desenvolvimento)
    static wxString GetProjectDir()
    {
        wxString exeDir = GetExecutableDir();
        wxFileName currentDir(exeDir);
        wxArrayString dirs = currentDir.GetDirs();

        if (!dirs.IsEmpty() && (dirs.Last() == wxT("Debug") || dirs.Last() == wxT("Release")))
        {
            // Estamos em bin/Debug ou bin/Release
            currentDir.RemoveLastDir(); // Remove Debug ou Release
            currentDir.RemoveLastDir(); // Remove bin
            return currentDir.GetPath() + wxFileName::GetPathSeparator();
        }

        // Em produção, retornar diretório do executável
        return exeDir;
    }

    // Obter diretório de recursos
    static wxString GetResourcesDir()
    {
        #ifdef __WXDEBUG__
            // Em modo debug, tentar usar resources da raiz do projeto
            wxString projectResources = GetProjectDir() + wxT("resources") + wxFileName::GetPathSeparator();
            if (wxDirExists(projectResources))
            {
                return projectResources;
            }
        #endif

        // Usar resources junto ao executável
        return GetExecutableDir() + wxT("resources") + wxFileName::GetPathSeparator();
    }

    // Obter diretório de ícones
    static wxString GetIconsDir()
    {
        return GetResourcesDir() + wxT("icons") + wxFileName::GetPathSeparator();
    }

    // Obter caminho do banco de dados
    static wxString GetDatabasePath()
    {
        // Criar diretório data se não existir
        wxString dataDir = GetExecutableDir() + wxT("data") + wxFileName::GetPathSeparator();
        if (!wxDirExists(dataDir))
        {
            wxMkdir(dataDir, wxS_DIR_DEFAULT);
        }

        return dataDir + wxT("familybudgets.db");
    }

    // Obter caminho do schema SQL
    static wxString GetSchemaPath()
    {
        return GetResourcesDir() + wxT("database") + wxFileName::GetPathSeparator() + wxT("schema.sql");
    }

    // Copiar recursos do projeto para o executável
    static bool CopyResourcesIfNeeded()
    {
        wxString sourceResourcesDir = GetProjectDir() + wxT("resources") + wxFileName::GetPathSeparator();
        wxString destResourcesDir = GetExecutableDir() + wxT("resources") + wxFileName::GetPathSeparator();

        // Se o diretório de origem não existir, não há o que copiar
        if (!wxDirExists(sourceResourcesDir))
        {
            return false;
        }

        // Criar diretório de destino se não existir
        if (!wxDirExists(destResourcesDir))
        {
            if (!wxMkdir(destResourcesDir))
            {
                return false;
            }
        }

        // Copiar database
        CopyDirectory(sourceResourcesDir + wxT("database"),
                     destResourcesDir + wxT("database"));

        // Copiar icons
        CopyDirectory(sourceResourcesDir + wxT("icons"),
                     destResourcesDir + wxT("icons"));

        return true;
    }

private:
    // Função auxiliar para copiar diretórios recursivamente
    static bool CopyDirectory(const wxString& source, const wxString& dest)
    {
        if (!wxDirExists(source))
        {
            return false;
        }

        // Criar diretório de destino
        if (!wxDirExists(dest))
        {
            if (!wxMkdir(dest))
            {
                return false;
            }
        }

        // Copiar todos os arquivos
        wxDir dir(source);
        if (!dir.IsOpened())
            return false;

        wxString filename;
        bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);

        while (cont)
        {
            wxString sourceFile = source + wxFileName::GetPathSeparator() + filename;
            wxString destFile = dest + wxFileName::GetPathSeparator() + filename;

            wxCopyFile(sourceFile, destFile, true);

            cont = dir.GetNext(&filename);
        }

        return true;
    }
};

#endif // PATHUTILS_H

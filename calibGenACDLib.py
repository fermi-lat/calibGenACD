# $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/calibGenACDLib.py,v 1.5 2014/02/14 03:28:29 echarles Exp $
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['calibGenACD'])
    env.Tool('xmlBaseLib')
    env.Tool('CalibDataLib')
    #env.Tool('AcdDigiLib')
    #env.Tool('mootCoreLib')
    env.Tool('calibUtilLib')
    env.Tool('overlayRootDataLib')
    env.Tool('reconRootDataLib')
    env.Tool('digiRootDataLib')
    env.Tool('commonRootDataLib')
    env.Tool('addLibrary', library = env['rootLibs'])
    env.Tool('addLibrary', library = env['rootGuiLibs'])
    env.Tool('addLibrary', library = env['minuitLibs'])
    env.Tool('addLibrary', library = env['xercesLibs'])
def exists(env):
    return 1;

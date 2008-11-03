# $Header$
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['calibGenACD'])
    env.Tool('xmlBaseLib')
    env.Tool('AcdDigiLib')
    env.Tool('mootCoreLib')
    env.Tool('calibUtilLib')
    env.Tool('digiRootDataLib')
    env.Tool('addLibrary', library = env['rootLibs'])
    env.Tool('reconRootDataLib')
    env.Tool('addLibrary', library = env['rootGuiLibs'])
def exists(env):
    return 1;

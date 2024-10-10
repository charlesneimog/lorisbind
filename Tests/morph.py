import lorisbind as loris
import librosa
import soundfile as sf


loris.analyzer_configure(0.2 * 440, 440)

# configure the analysis
fl_samples, fl_sr = librosa.load("Hp-ord-A4-mf.aif", sr=48000)
flute_sdif = loris.analyze(fl_samples, fl_sr)
fund_env = loris.createF0Estimate(flute_sdif, 400, 500, 0.1)
loris.channelize(flute_sdif, fund_env, 1)
loris.sift(flute_sdif)
loris.distill(flute_sdif)


cl_samples, cl_sr = librosa.load("cymbal.wav", sr=48000)
cl_sdif = loris.analyze(cl_samples, cl_sr)
fund_env = loris.createF0Estimate(cl_sdif, 400, 500, 0.1)
loris.channelize(cl_sdif, fund_env, 1)
loris.sift(cl_sdif)
loris.distill(cl_sdif)


env = loris.LinearEnvelope()
env.insertBreakpoint(2, 0)
env.insertBreakpoint(4.5, 1)

dest = loris.PartialList()

loris.morph(flute_sdif, cl_sdif, env, env, env, dest)

buff = dest.synthesize(48000)

sf.write("stereo_file.wav", buff, 48000)


#
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookDataSamples
# ------------------------------------------------------------------
#
# cmsRun pickEvent_cfg.py inputFiles=file1.root \
#       eventsToProcess=123592:334:755009,123592:23:392793,123592:42:79142 \
#       outputFile=output.root
#

import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing ('analysis')
# add a list of strings for events to process
options.register ('eventsToProcess',
                                  '',
                                  VarParsing.multiplicity.list,
                                  VarParsing.varType.string,
                                  "Events to process")
options.parseArguments()

process = cms.Process("PickEvent")
process.source = cms.Source ("PoolSource",
          fileNames = cms.untracked.vstring (options.inputFiles),
          eventsToProcess = cms.untracked.VEventRange (options.eventsToProcess)                               
)

process.Out = cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string (options.outputFile)
)

process.end = cms.EndPath(process.Out)

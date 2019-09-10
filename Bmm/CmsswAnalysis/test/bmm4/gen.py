import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("HFA")

# ----------------------------------------------------------------------
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )


# ----------------------------------------------------------------------
# -- Database configuration
#process.load("CondCore.DBCommon.CondDBCommon_cfi")
#process.load("CondCore.DBCommon.CondDBSetup_cfi")
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

# -- Conditions
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
#process.load("Configuration.StandardSequences.Geometry_cff")
process.load("RecoVertex.BeamSpotProducer.BeamSpot_cfi")
#process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

#version = os.environ.get('CMSSW_VERSION')
#if version.find('CMSSW_5_3') != -1:
#    globaltag = "START53_V7G::All"
#else:
#    globaltag = "93X_mc2017_realistic_v3"

#globaltag = "START70_V7A::All"
#print('==> GT = %s ' % globaltag)
#process.GlobalTag.globaltag = globaltag

# ----------------------------------------------------------------------
process.source = cms.Source(
 "PoolSource",
  fileNames = cms.untracked.vstring(
 #        "file:/mnt/t3nfs01/data01/shome/oozcelik/bmm/CMSSW_9_4_6_patch1/src/Bmm/CmsswAnalysis/test/bmm4/test_small.root"
"file:/mnt/t3nfs01/data01/shome/oozcelik/bmm/CMSSW_9_4_6_patch1/src/Bmm/CmsswAnalysis/test/bmm4/BPH-RunIIFall17GS-00002.root"
 )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')

# ----------------------------------------------------------------------
rootFileName = "gen.root"
process.tree = cms.EDAnalyzer(
    "HFTree",
    verbose        = cms.untracked.int32(10),
    printFrequency = cms.untracked.int32(1),
    requireCand    = cms.untracked.bool(False),
    fullGenBlock   = cms.untracked.bool(True),
    fileName       = cms.untracked.string(rootFileName)
    )

# ----------------------------------------------------------------------
process.stuffDump = cms.EDAnalyzer(
    "HFDumpStuff",
    verbose                  = cms.untracked.int32(0),
    PrimaryVertexLabel       = cms.untracked.InputTag("offlinePrimaryVertices"),
    PrimaryVertexTracksLabel = cms.untracked.InputTag("generalTracks")
    )

# ----------------------------------------------------------------------
process.load("Bmm.CmsswAnalysis.HFMCTruth_cff")
process.genDump.verbose = cms.untracked.int32(3)

# ----------------------------------------------------------------------
process.p = cms.Path(
    process.stuffDump*
    process.MCTruthSequence*
    process.tree
)

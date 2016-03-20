## \file
## \ingroup tutorial_pyroot
## This macro displays the Tree data structures
##
## \macro_image
## \macro_code
##
## \author Wim Lavrijsen

from ROOT import TCanvas, TPaveLabel, TPaveText, TPavesText, TText
from ROOT import TArrow, TLine
from ROOT import gROOT, gBenchmark

#gROOT.Reset()

c1 = TCanvas('c1','Tree Data Structure',200,10,750,940)
c1.Range(0,-0.1,1,1.15)

gBenchmark.Start('tree')

branchcolor = 26
leafcolor   = 30
basketcolor = 42
offsetcolor = 43
#title = TPaveLabel(.3,1.05,.8,1.13,c1.GetTitle())
title = TPaveLabel(.3,1.05,.8,1.13,'Tree Data Structure')
title.SetFillColor(16)
title.Draw()
tree = TPaveText(.01,.75,.15,1.00)
tree.SetFillColor(18)
tree.SetTextAlign(12)
tnt = tree.AddText('Tree')
tnt.SetTextAlign(22)
tnt.SetTextSize(0.030)
tree.AddText('fScanField')
tree.AddText('fMaxEventLoop')
tree.AddText('fMaxVirtualSize')
tree.AddText('fEntries')
tree.AddText('fDimension')
tree.AddText('fSelectedRows')
tree.Draw()
farm = TPavesText(.01,1.02,.15,1.1,9,'tr')
tfarm = farm.AddText('CHAIN')
tfarm.SetTextSize(0.024)
farm.AddText('Collection')
farm.AddText('of Trees')
farm.Draw()
link = TLine(.15,.92,.80,.92)
link.SetLineWidth(2)
link.SetLineColor(1)
link.Draw()
link.DrawLine(.21,.87,.21,.275)
link.DrawLine(.23,.87,.23,.375)
link.DrawLine(.25,.87,.25,.775)
link.DrawLine(.41,.25,.41,-.025)
link.DrawLine(.43,.25,.43,.075)
link.DrawLine(.45,.25,.45,.175)
branch0 = TPaveLabel(.20,.87,.35,.97,'Branch 0')
branch0.SetTextSize(0.35)
branch0.SetFillColor(branchcolor)
branch0.Draw()
branch1 = TPaveLabel(.40,.87,.55,.97,'Branch 1')
branch1.SetTextSize(0.35)
branch1.SetFillColor(branchcolor)
branch1.Draw()
branch2 = TPaveLabel(.60,.87,.75,.97,'Branch 2')
branch2.SetTextSize(0.35)
branch2.SetFillColor(branchcolor)
branch2.Draw()
branch3 = TPaveLabel(.80,.87,.95,.97,'Branch 3')
branch3.SetTextSize(0.35)
branch3.SetFillColor(branchcolor)
branch3.Draw()
leaf0 = TPaveLabel(.4,.75,.5,.8,'Leaf 0')
leaf0.SetFillColor(leafcolor)
leaf0.Draw()
leaf1 = TPaveLabel(.6,.75,.7,.8,'Leaf 1')
leaf1.SetFillColor(leafcolor)
leaf1.Draw()
leaf2 = TPaveLabel(.8,.75,.9,.8,'Leaf 2')
leaf2.SetFillColor(leafcolor)
leaf2.Draw()
firstevent = TPaveText(.4,.35,.9,.4)
firstevent.AddText('First event of each basket')
firstevent.AddText('Array of fMaxBaskets Integers')
firstevent.SetFillColor(basketcolor)
firstevent.Draw()
basket0 = TPaveLabel(.4,.25,.5,.3,'Basket 0')
basket0.SetFillColor(basketcolor)
basket0.Draw()
basket1 = TPaveLabel(.6,.25,.7,.3,'Basket 1')
basket1.SetFillColor(basketcolor)
basket1.Draw()
basket2 = TPaveLabel(.8,.25,.9,.3,'Basket 2')
basket2.SetFillColor(basketcolor)
basket2.Draw()

offset = TPaveText(.55,.15,.9,.2)
offset.AddText('Offset of events in fBuffer')
offset.AddText('Array of fEventOffsetLen Integers')
offset.AddText('(if variable length structure)')
offset.SetFillColor(offsetcolor)
offset.Draw()
buffer = TPaveText(.55,.05,.9,.1)
buffer.AddText('Basket buffer')
buffer.AddText('Array of fBasketSize chars')
buffer.SetFillColor(offsetcolor)
buffer.Draw()
zipbuffer = TPaveText(.55,-.05,.75,.0)
zipbuffer.AddText('Basket compressed buffer')
zipbuffer.AddText('(if compression)')
zipbuffer.SetFillColor(offsetcolor)
zipbuffer.Draw()
ar1 = TArrow()
ar1.SetLineWidth(2)
ar1.SetLineColor(1)
ar1.SetFillStyle(1001)
ar1.SetFillColor(1)
ar1.DrawArrow(.21,.275,.39,.275,0.015,'|>')
ar1.DrawArrow(.23,.375,.39,.375,0.015,'|>')
ar1.DrawArrow(.25,.775,.39,.775,0.015,'|>')
ar1.DrawArrow(.50,.775,.59,.775,0.015,'|>')
ar1.DrawArrow(.70,.775,.79,.775,0.015,'|>')
ar1.DrawArrow(.50,.275,.59,.275,0.015,'|>')
ar1.DrawArrow(.70,.275,.79,.275,0.015,'|>')
ar1.DrawArrow(.45,.175,.54,.175,0.015,'|>')
ar1.DrawArrow(.43,.075,.54,.075,0.015,'|>')
ar1.DrawArrow(.41,-.025,.54,-.025,0.015,'|>')
ldot = TLine(.95,.92,.99,.92)
ldot.SetLineStyle(3)
ldot.Draw()
ldot.DrawLine(.9,.775,.99,.775)
ldot.DrawLine(.9,.275,.99,.275)
ldot.DrawLine(.55,.05,.55,0)
ldot.DrawLine(.9,.05,.75,0)
pname = TText(.46,.21,'fEventOffset')
pname.SetTextFont(72)
pname.SetTextSize(0.018)
pname.Draw()
pname.DrawText(.44,.11,'fBuffer')
pname.DrawText(.42,.01,'fZipBuffer')
pname.DrawText(.26,.81,'fLeaves = TObjArray of TLeaf')
pname.DrawText(.24,.40,'fBasketEvent')
pname.DrawText(.22,.31,'fBaskets = TObjArray of TBasket')
pname.DrawText(.20,1.0,'fBranches = TObjArray of TBranch')
ntleaf = TPaveText(0.30,.42,.62,.7)
ntleaf.SetTextSize(0.014)
ntleaf.SetFillColor(leafcolor)
ntleaf.SetTextAlign(12)
ntleaf.AddText('fLen: number of fixed elements')
ntleaf.AddText('fLenType: number of bytes of data type')
ntleaf.AddText('fOffset: relative to Leaf0-fAddress')
ntleaf.AddText('fNbytesIO: number of bytes used for I/O')
ntleaf.AddText('fIsPointer: True if pointer')
ntleaf.AddText('fIsRange: True if leaf has a range')
ntleaf.AddText('fIsUnsigned: True if unsigned')
ntleaf.AddText('*fLeafCount: points to Leaf counter')
ntleaf.AddText(' ')
ntleaf.AddLine(0,0,0,0)
ntleaf.AddText('fName = Leaf name')
ntleaf.AddText('fTitle = Leaf type (see Type codes)')
ntleaf.Draw()
type = TPaveText(.65,.42,.95,.7)
type.SetTextAlign(12)
type.SetFillColor(leafcolor)
type.AddText(' ')
type.AddText('C : a character string')
type.AddText('B : an 8 bit signed integer')
type.AddText('b : an 8 bit unsigned integer')
type.AddText('S : a 16 bit signed short integer')
type.AddText('s : a 16 bit unsigned short integer')
type.AddText('I : a 32 bit signed integer')
type.AddText('i : a 32 bit unsigned integer')
type.AddText('F : a 32 bit floating point')
type.AddText('D : a 64 bit floating point')
type.AddText('TXXXX : a class name TXXXX')
type.Draw()
typecode = TPaveLabel(.7,.68,.9,.72,'fType codes')
typecode.SetFillColor(leafcolor)
typecode.Draw()
ldot.DrawLine(.4,.75,.30,.7)
ldot.DrawLine(.5,.75,.62,.7)
ntbasket = TPaveText(0.02,-0.07,0.35,.25)
ntbasket.SetFillColor(basketcolor)
ntbasket.SetTextSize(0.014)
ntbasket.SetTextAlign(12)
ntbasket.AddText('fNbytes: Size of compressed Basket')
ntbasket.AddText('fObjLen: Size of uncompressed Basket')
ntbasket.AddText('fDatime: Date/Time when written to store')
ntbasket.AddText('fKeylen: Number of bytes for the key')
ntbasket.AddText('fCycle : Cycle number')
ntbasket.AddText('fSeekKey: Pointer to Basket on file')
ntbasket.AddText('fSeekPdir: Pointer to directory on file')
ntbasket.AddText("fClassName: 'TBasket'")
ntbasket.AddText('fName: Branch name')
ntbasket.AddText('fTitle: Tree name')
ntbasket.AddText(' ')
ntbasket.AddLine(0,0,0,0)
ntbasket.AddText('fNevBuf: Number of events in Basket')
ntbasket.AddText('fLast: pointer to last used byte in Basket')
ntbasket.Draw()
ldot.DrawLine(.4,.3,0.02,0.25)
ldot.DrawLine(.5,.25,0.35,-.07)
ldot.DrawLine(.5,.3,0.35,0.25)
ntbranch = TPaveText(0.02,0.40,0.18,0.68)
ntbranch.SetFillColor(branchcolor)
ntbranch.SetTextSize(0.015)
ntbranch.SetTextAlign(12)
ntbranch.AddText('fBasketSize')
ntbranch.AddText('fEventOffsetLen')
ntbranch.AddText('fMaxBaskets')
ntbranch.AddText('fEntries')
ntbranch.AddText('fAddress of Leaf0')
ntbranch.AddText(' ')
ntbranch.AddLine(0,0,0,0)
ntbranch.AddText('fName: Branchname')
ntbranch.AddText('fTitle: leaflist')
ntbranch.Draw()
ldot.DrawLine(.2,.97,.02,.68)
ldot.DrawLine(.35,.97,.18,.68)
ldot.DrawLine(.35,.87,.18,.40)
basketstore = TPavesText(.8,-0.088,0.952,-0.0035,7,'tr')
basketstore.SetFillColor(28)
basketstore.AddText('Baskets')
basketstore.AddText('Stores')
basketstore.Draw()
c1.Update()

gBenchmark.Show('tree')

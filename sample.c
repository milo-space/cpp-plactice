a = $Teapot001
b = $Teapot002
m = a.modifiers[1]

ctm = getModContextTM a m
mn  = getModContextBBoxMin a m
mx  = getModContextBBoxMax a m

wtm = (inverse ctm) * a.objectTransform

addModifier b m

ctm = inverse (wtm * inverse b.objectTransform)

setModContextTM b m ctm
setModContextBBox b m mn mx

redrawViews()

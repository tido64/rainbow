#!/bin/bash
prev=2012
next=13
for i in demos lua src; do
	find $i -type f -print0 | xargs -0 sed -i "s/-.. Bifrost Entertainment/-$next Bifrost Entertainment/g"
	find $i -type f -print0 | xargs -0 sed -i "s/$prev Bifrost Entertainment/$prev-$next Bifrost Entertainment/g"
done

#pragma once

template <typename Element> 
int find_element(Element* list, Element seek, int length)
{
	for (int i = 0; i < length; i++)
		if (list[i] == seek)
			return i;
	return -1;
}

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "AnnotsHandler.h"

using namespace merge_lib;
void AnnotsHandler::_processObjectContent(unsigned int startOfPageElement)
{
   unsigned int endOfAnnots = _findEndOfElementContent(startOfPageElement);
   _annotations = _page->getSortedByPositionChildren(startOfPageElement, endOfAnnots);

   // sometimes annotations array is defined not directly in page object content but 
   // in referred object.
   // Example:
   // /Annots 12 0 R
   // ...
   // 12 0 obj[ 13 0 R 14 0 R] endobj
   // So in this case _annotations contains one element which content is array of annotation 
   // references.

   // lets check the content if it is really annotation object
   if( _annotations.size() )
   {
      Object * child = _annotations[0];
      std::string childContent = child->getObjectContent();
      if( Parser::findToken(childContent,"/Rect") == std::string::npos &&
         Parser::findToken(childContent,"/Subtype") == std::string::npos )
      {
         // this was not Annotation but reference to array 
         // of annotations 
         _annotations.erase(_annotations.begin(),_annotations.end());
         size_t begin = 0;
         size_t end  = childContent.size()-1;
         _annotations = child->getSortedByPositionChildren(begin,end);
         
         // lets update the parent in order to skip this intermediate child
         // For now it seems to be not needed, but code below is working
         // it is pity to delete
         /*
         _page->removeChild(child);
         _page->eraseContent(startOfPageElement,endOfAnnots-startOfPageElement);

         std::string annotTag= "/Annots [";
         _page->insertToContent(startOfPageElement,annotTag);
         size_t pos = startOfPageElement + annotTag.size();

         for(size_t i = 0;i<_annotations.size();i++)
         {
            std::stringstream newContent;
            newContent<<_annotations[i]->getObjectNumber()<<" 0 R ";

            std::vector<unsigned int> posVec;
            posVec.push_back(pos);

            _page->insertToContent(pos,newContent.str());
            _page->addChild(_annotations[i],posVec);
            pos += newContent.str().size();
         }
         _page->insertToContent(pos," ]\n");
         */
      }
   }
}


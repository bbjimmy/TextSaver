/* 
**
** A simple screensaver, displays the text at random locations.
**
** Version: 2.0
**
** 
** Copyright (c) 2002 Marcus Overhagen, 2004-2013 Jim Saxton
 ... FAT ELK SOFTWARE 

Permission is hereby granted, free of charge, to any person obtaining 
a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
*/

#include <ScreenSaver.h>
#include <View.h>
#include <StringView.h>
#include <Font.h>
#include <stdlib.h>
#include <String.h>
#include <TextControl.h>


BString 	fText;

bool			clear;

class SetupView : public BView
{
	
public:
	SetupView(BRect frame, const char *name)
	 : BView(frame, name, 0, B_FOLLOW_ALL)
	{
	}

	void AttachedToWindow()
	{
		BMessage *msg;
		SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		BRect r(10, 5, 270, 25);
		AddChild(new BStringView(r, B_EMPTY_STRING, "TextSaver by Jim Saxton"));
		r.OffsetBy(0,25);
		AddChild(new BStringView(r, B_EMPTY_STRING, "based on:"));
		r.OffsetBy(0,15);
		AddChild(new BStringView(r, B_EMPTY_STRING, "BeOS ScreenSaver by Marcus Overhagen"));
		
		r.OffsetBy(0,35);
		AddChild(new BStringView(r, B_EMPTY_STRING, "Enter Text:"));
		msg=new BMessage('text');
		r.OffsetBy(0,25);
		AddChild(_text=new BTextControl(r, B_EMPTY_STRING,NULL,fText.String(),msg));
		_text->SetTarget(this);
		
		

	}

	void MessageReceived(BMessage *msg)
	{

		switch(msg->what)
		{
			case 'text' :
				clear=true;
				fText.SetTo(_text->Text());	
			break;
			
			default :
				BView::MessageReceived(msg);
			break;
		}
	}
	
	private:
		BTextControl*	_text;
		
};


class ScreenSaver : public BScreenSaver
{
public:
	ScreenSaver(BMessage *archive, image_id);
	void Draw(BView *view, int32 frame);
	void StartConfig(BView *view);
	status_t StartSaver(BView *view, bool preview);
	
	status_t SaveState(BMessage *into) const;
private:
	//const char *fText;
	int red;
	int green;
	int blue;
	int colors;
	float fX;
	float fY;
	float fSizeX;
	float fSizeY;
	float fTextHeight;
	float fTextWith;
	bool fIsPreview;
	
	
};

BScreenSaver *instantiate_screen_saver(BMessage *msg, image_id image) 
{ 
	return new ScreenSaver(msg, image);
} 

ScreenSaver::ScreenSaver(BMessage *archive, image_id id) :
	BScreenSaver(archive, id),
	fX(0),
	fY(0)
{
	clear=true;
	//fText.SetTo("Haiku");
	
	archive->FindString("text",&fText);
	if(fText=="") fText.SetTo("Your Text");
	
}

status_t 
ScreenSaver::SaveState(BMessage *into) const
{
	into->AddString("text",fText);
	
	
	return B_OK;
}

void 
ScreenSaver::StartConfig(BView *view) 
{ 
	view->AddChild(new SetupView(view->Bounds(),""));
} 

status_t 
ScreenSaver::StartSaver(BView *view, bool preview)
{
	// save view dimensions and preview mode
	fIsPreview = preview;
	fSizeX = view->Bounds().Width() -40;
	fSizeY = view->Bounds().Height();
	
	// set a new font, about 1/8th of view height, and bold
	BFont font;
	view->GetFont(&font);
	//font.SetSize(fSizeY / 8);
	
	
	// find out space needed for text display
	BRect rect;
	escapement_delta delta;
	delta.nonspace = 0;
	delta.space = 0;
	const char* x=fText.String();
	int32 chars = fText.CountChars();
	font.SetSize(fSizeY/chars);
	if (chars > 20)
	font.SetSize(25);
	font.SetFace(B_BOLD_FACE);
	view->SetFont(&font);
	
	font.GetBoundingBoxesForStrings(&(x),1,B_SCREEN_METRIC,&delta,&rect);
	fTextHeight = rect.Height();
	fTextWith = rect.Width();
	
	// seed the random number generator
	srand((int)system_time());
	

	
	return B_OK;
}



void 
ScreenSaver::Draw(BView *view, int32 frame)
{
	if (frame == 0 || clear) { 
		// fill with black on first frame
		view->SetLowColor(0, 0, 0); 
		view->FillRect(view->Bounds(), B_SOLID_LOW); 
		//clear=false;
	} else {
		// erase old text on all other frames
		view->SetHighColor(0,0,0);
		view->DrawString(fText.String(),BPoint(fX,fY));
	}

	// find some new text coordinates
	
	float shear = 60.0;
	
	BFont font;
	float ofset=rand()%70;
	shear=shear+ofset;
	font.SetShear(shear);
	view->SetFont(&font, B_FONT_SHEAR);
	
	fX = rand() % int(fSizeX - fTextWith) + 20;
	fY = rand() % int(fSizeY - fTextHeight - (fIsPreview ? 2 : 20)) + fTextHeight;

	// draw new text
	red = rand() % 255;
	
	blue = rand() % 255;
	
	green = rand () % 255;
	
	colors=red;
	colors=colors+blue;
	colors = colors+green;
	if (colors < 250)
	{
	red=255;
	}
	
	
	
	view->SetHighColor(red,green,blue);
	view->DrawString(fText.String(),BPoint(fX,fY));

	// randomize time until next update (preview mode is faster)
	SetTickSize(((rand() % 4) + 1) * (fIsPreview ? 300000 : 1000000));
}


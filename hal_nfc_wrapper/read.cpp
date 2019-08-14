#include "read.h"
#include <nan.h>
#include "nfc.h"
#include "data/info.h"
#include "data/page.h"
#include "data/ndef.h"

#include <iostream>

class AsyncReader : public Nan::AsyncWorker {
public:
  int nfc_status, info_status, pages_status, page_status, ndef_status;
  readOptions options;
  std::vector<uint8_t> nfc_page;

  AsyncReader(Nan::Callback *callback, readOptions options )
    : Nan::AsyncWorker(callback) {
      this->options = options;
    }

  // Execute non-blocking code. **DO NOT MAKE V8 CALLS HERE!**
  void Execute() {
    // Prevent async calls overlapping NFC
    if (!nfc_active){
      nfc_active = true;

      // Get requested NFC data //
      if (options.info) {
        nfc_status = nfc.Activate();
        info_status = nfc.ReadInfo(&nfc_data.info);
        nfc.Deactivate();
      }
      if (options.pages) {
        nfc_status = nfc.Activate();
        pages_status = nfc.mful.ReadPages(&nfc_data.pages);
        nfc.Deactivate();
      }
      if (options.page != -1){
        nfc_status = nfc.Activate();
        nfc_page = nfc.mful.ReadPage(options.page);
        nfc.Deactivate();
      }
      if (options.ndef){
        nfc_status = nfc.Activate();
        ndef_status = nfc.ndef.Read(&nfc_data.ndef);
        nfc.Deactivate();
      }
    }

    // Run HandleErrorCallback()
    else {
      this->SetErrorMessage("Error: NFC Was Busy Reading/Writing!");
      return;
    }
  }

  void HandleOKCallback() {
    // Prevent V8 objects from being garbage collected
    Nan::HandleScope scope;

    // Return scanned NFC data
    v8::Local<v8::Object> tag_data = Nan::New<v8::Object>();

    // Create NFC data & add read status to each data object //
    // * Info
    if (options.info) {
      v8::Local<v8::Object> info_data = info_data_js();
      Nan::Set(info_data, Nan::New("status").ToLocalChecked(), Nan::New(info_status));
      Nan::Set(tag_data, Nan::New("info").ToLocalChecked(), info_data);
    }
    // * Pages
    if (options.pages) {
      v8::Local<v8::Object> pages_data = pages_data_js();
      Nan::Set(pages_data, Nan::New("status").ToLocalChecked(), Nan::New(pages_status));
      Nan::Set(tag_data, Nan::New("pages").ToLocalChecked(), pages_data);
    }
    // * Page
    if (options.page != -1){
      // Instead of a status, page will return an empty array if it fails & a populated one if it passes.
      Nan::Set(tag_data, Nan::New("page").ToLocalChecked(), page_data_js(nfc_page));
    }
    // * NDEF
    if (options.ndef){
      v8::Local<v8::Object> ndef_data = ndef_data_js();
      Nan::Set(ndef_data, Nan::New("status").ToLocalChecked(), Nan::New(ndef_status));
      Nan::Set(tag_data, Nan::New("ndef").ToLocalChecked(), ndef_data);
    }

    // Callback Parameters
    int argCount = 2;
    v8::Local<v8::Value> argv[] = {
      Nan::New(nfc_status),
      tag_data
    };

    // Start callback
    nfc_active = false;
    Nan::Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), argCount, argv);
  }

  // Called if NFC was busy
  void HandleErrorCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
      Nan::New(-999)// status code we made up for busy NFC
    };
    Nan::Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), 1, argv);
  }
};

NAN_METHOD(read){
  //TODO: parse user args

  // Default args
  readOptions options = {false,false,-1,false};

  // Parse user args
  if (!info[0]->IsFunction()) {Nan::ThrowTypeError(".read argument 0 must be a function");return;}
  if (info[1]->IsObject()) {
    // v8::Local<v8::Object> read_options = info[1]->ToObject();
    v8::Local<v8::Object> read_options = Nan::To<v8::Object>(info[1]).ToLocalChecked();

    v8::Local<v8::String> info_prop  = Nan::New("info").ToLocalChecked();
    v8::Local<v8::String> pages_prop = Nan::New("pages").ToLocalChecked();
    v8::Local<v8::String> page_prop  = Nan::New("page").ToLocalChecked();
    v8::Local<v8::String> ndef_prop  = Nan::New("ndef").ToLocalChecked();

    // Set read options
    if (Nan::HasOwnProperty(read_options, info_prop).FromJust()) {

    // v8::Local<v8::Value> info_check = Nan::Get(read_options, info_prop);

    v8::Local<v8::Value> test1 = Nan::Get(read_options, info_prop).ToLocalChecked();
    bool test2 = test1;
    // bool test = Nan::To<bool>(Nan::Get(read_options, info_prop).ToLocalChecked()).FromJust();

      
    //   // bool test = info_check->BooleanValue();

    //   // bool test = Nan::New(Nan::To<bool>(info_check).FromJust());
    //   std::cout << test << std::endl;
    }
  }

  Nan::Callback *callback = new Nan::Callback(
    Nan::To<v8::Function>(info[0]).ToLocalChecked()
  );

  Nan::AsyncQueueWorker(new AsyncReader(callback, options));
}
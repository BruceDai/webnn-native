// Copyright 2021 The WebNN-native Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "webnn_wire/client/context.h"

#include "webnn_wire/client/Client.h"
#include "webnn_wire/WireCmd_autogen.h"

namespace webnn_wire { namespace client {

    void Context::PushErrorScope(MLErrorFilter filter) {
        mErrorScopeStackSize++;

        ContextPushErrorScopeCmd cmd;
        cmd.self = ToAPI(this);
        cmd.filter = filter;

        client->SerializeCommand(cmd);
    }

    bool Context::PopErrorScope(MLErrorCallback callback, void* userdata) {
        if (mErrorScopeStackSize == 0) {
            return false;
        }
        mErrorScopeStackSize--;

        if (client->IsDisconnected()) {
            callback(MLErrorType_DeviceLost, "GPU device disconnected", userdata);
            return true;
        }

        uint64_t serial = mErrorScopeRequestSerial++;
        ASSERT(mErrorScopes.find(serial) == mErrorScopes.end());

        mErrorScopes[serial] = {callback, userdata};

        ContextPopErrorScopeCmd cmd;
        cmd.contextId = this->id;
        cmd.requestSerial = serial;

        client->SerializeCommand(cmd);

        return true;
    }

    void Context::SetUncapturedErrorCallback(MLErrorCallback callback, void* userdata) {
    }

}}  // namespace webnn_wire::client

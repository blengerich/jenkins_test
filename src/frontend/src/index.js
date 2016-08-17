import jwt from 'jsonwebtoken'
import React from 'react'
import { render } from 'react-dom'
import { Provider } from 'react-redux'
import { Router, Route, hashHistory } from 'react-router'
import injectTapEventPlugin from 'react-tap-event-plugin'

injectTapEventPlugin()

import configureStore from './store/configureStore'
import { requireAuthentication } from './components/AuthenticatedComponent'
import { addDevTools } from './components/WithDevTools'
import GMAppContainer from './components/GMAppContainer'
import GMDataList from './components/GMDataList'
import GMMatrixVisualization from './components/GMMatrixVisualization'
import GMLoginContainer from './components/GMLoginContainer'
import GMCreateAccountContainer from './components/GMCreateAccountContainer'
import { setInitialUserState } from './actions'
import { getToken, verifyToken, removeToken } from './middleware/token'

const store = configureStore()

const token = getToken()
verifyToken(token) ? store.dispatch(setInitialUserState(token)) : removeToken()

render(
  <Provider store={store}>
    <Router history={hashHistory}>
      <Route path='/login' component={addDevTools(GMLoginContainer)} />
      <Route path='/register' component={addDevTools(GMCreateAccountContainer)} />
      <Route path='/' component={addDevTools(requireAuthentication(GMAppContainer))}>
        <Route path='data/:id' component={GMDataList} />
        <Route
          path='visualization/matrix/:markerLabelId/:traitLabelId/:resultId'
          component={GMMatrixVisualization}
        />
      </Route>
    </Router>
  </Provider>,
  document.getElementById('gm-app')
)

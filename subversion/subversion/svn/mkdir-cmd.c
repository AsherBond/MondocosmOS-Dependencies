/*
 * mkdir-cmd.c -- Subversion mkdir command
 *
 * ====================================================================
 *    Licensed to the Apache Software Foundation (ASF) under one
 *    or more contributor license agreements.  See the NOTICE file
 *    distributed with this work for additional information
 *    regarding copyright ownership.  The ASF licenses this file
 *    to you under the Apache License, Version 2.0 (the
 *    "License"); you may not use this file except in compliance
 *    with the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing,
 *    software distributed under the License is distributed on an
 *    "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *    KIND, either express or implied.  See the License for the
 *    specific language governing permissions and limitations
 *    under the License.
 * ====================================================================
 */

/* ==================================================================== */



/*** Includes. ***/

#include "svn_pools.h"
#include "svn_client.h"
#include "svn_error.h"
#include "svn_path.h"
#include "cl.h"

#include "svn_private_config.h"


/*** Code. ***/

/* This implements the `svn_opt_subcommand_t' interface. */
svn_error_t *
svn_cl__mkdir(apr_getopt_t *os,
              void *baton,
              apr_pool_t *pool)
{
  svn_cl__opt_state_t *opt_state = ((svn_cl__cmd_baton_t *) baton)->opt_state;
  svn_client_ctx_t *ctx = ((svn_cl__cmd_baton_t *) baton)->ctx;
  apr_array_header_t *targets;
  svn_error_t *err;

  SVN_ERR(svn_cl__args_to_target_array_print_reserved(&targets, os,
                                                      opt_state->targets,
                                                      ctx, FALSE, pool));

  if (! targets->nelts)
    return svn_error_create(SVN_ERR_CL_INSUFFICIENT_ARGS, 0, NULL);

  SVN_ERR(svn_cl__assert_homogeneous_target_type(targets));

  if (! svn_path_is_url(APR_ARRAY_IDX(targets, 0, const char *)))
    {
      ctx->log_msg_func3 = NULL;
      if (opt_state->message || opt_state->filedata || opt_state->revprop_table)
        {
          return svn_error_create
            (SVN_ERR_CL_UNNECESSARY_LOG_MESSAGE, NULL,
             _("Local, non-commit operations do not take a log message "
               "or revision properties"));
        }
    }
  else
    {
      SVN_ERR(svn_cl__make_log_msg_baton(&(ctx->log_msg_baton3), opt_state,
                                         NULL, ctx->config, pool));
    }

  SVN_ERR(svn_cl__eat_peg_revisions(&targets, targets, pool));

  err = svn_client_mkdir4(targets, opt_state->parents,
                          opt_state->revprop_table, svn_cl__print_commit_info,
                          NULL, ctx, pool);

  if (ctx->log_msg_func3)
    err = svn_cl__cleanup_log_msg(ctx->log_msg_baton3, err, pool);

  if (err)
    {
      if (err->apr_err == APR_EEXIST)
        return svn_error_quick_wrap
          (err, _("Try 'svn add' or 'svn add --non-recursive' instead?"));
      else if (!(opt_state->parents) &&
               (APR_STATUS_IS_ENOENT(err->apr_err) || /* in wc */
                err->apr_err == SVN_ERR_FS_NOT_DIRECTORY ||
                err->apr_err == SVN_ERR_FS_NOT_FOUND /* all ra layers */))
        return svn_error_quick_wrap
          (err, _("Try 'svn mkdir --parents' instead?"));
      else
        return svn_error_trace(err);
    }

  return SVN_NO_ERROR;
}

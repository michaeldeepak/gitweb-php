<?php
	/** @file
	 * Functions used by git control.
	 */

	/**
	 *  Get list of remote (branches) for a project.
	 *  
	 * @param string $project
	 * @return multitype:
	 */
	function getRemoteBranches($project="UniteZOne")
	{
		$output = runGit( "branch -r" );
		array_walk($output, "removeText","origin/");

		return $output;
	}
	
	/**
	 * remove the text and trim the spaces
	 * @param string $subject
	 * @param int $index
	 * @param string $text
	 */
	function removeText( &$subject,  $index, $text = "" )
	{
		if( !empty( $text ) ){
			$subject = trim( str_ireplace($text, "", $subject) );
		}else{
			$subject = trim( $subject );
		}
	}
	
	/**
	 * Get the current branch checked out for the projext
	 * return array of branches
	 */
	function getCurrentBranch($project="UniteZOne")
	{
		//$output = runGit("symbolic-ref -q HEAD ");
		$output = runGit( "branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ \\1/'" );
	
		return trim( reset( $output ) );
	}
	
	/**
	 * checkout master and fetch all remote branches
	 * @param unknown_type $project
	 */
	function refreshRemoteBranches( $project="UniteZOne")
	{
		$currentBranch = getCurrentBranch( $project );
		
		//don't switch branches if current branch is master
		if( $currentBranch !== "master")		runGit( "checkout master");
		
		runGit( "fetch");
		runGit( "merge origin/master");
		runGit( "remote prune origin" );
		if( $currentBranch !== "master") runGit( "checkout {$currentBranch} ");
		
		return TRUE;
	
	}
	
	/**
	 * get the local branches excluding the text(*)
	 * @param string $project
	 */
	function getLocalBranches( $project="UniteZOne" )
	{
		$output = runGit("branch", $project);	
		array_walk($output, "removeText","*");
		return $output;
	}
	
	/**
	 * switches the branch to the branchname provided
	 * 
	 * @param string $branchName
	 * @param string $project
	 */
	function switchBranch( $branchName,  $project="UniteZOne" )
	{
		$currentBranch = getCurrentBranch( $project );
		$branchName = trim( $branchName );
		
		//Only switch if the branchname is different than current branch
		if( $currentBranch !== $branchName){
			$localBranch = getLocalBranches( $project);
			
			// switch branch if its present locally ( usually only master)
			if( in_array($branchName, $localBranch)){
				runGit("checkout $branchName", $project);
				refreshRemoteBranches( $project );
				if( $branchName !== "master"){
					runGit("pull origin/$branchName", $project);
				}
			}else{
				//switch branches from remote
				refreshRemoteBranches( $project );
				runGit("checkout -b $branchName origin/$branchName", $project);
			}
		}
		
		$output = checkBranches();
		
		// switch back if the branch is dirty
		if( !empty( $output )){
			runGit( "checkout $currentBranch");
			removeAllLocalBranches( array( $currentBranch ), $project );
			echo "switched back to $currentBranch";
		}else {
			if( $branchName !== "master" ){
				removeAllLocalBranches( array( $branchName ), $project );
			}else{
				removeAllLocalBranches( '', $project);
			}
		}
	}
	
	/**
	 * Remove all the local branches excluding the master and current working branch
	 * @param array $ignored branches to not delete
	 * @param string $project
	 */
	function removeAllLocalBranches( $ignored,  $project="UniteZOne" )
	{
		if( !empty( $ignored ) && is_array( $ignored )  ){
			if( !in_array("master", $ignored ) )	$ignored[] = "master";
		}else{
			$ignored[] = array("master");
		}
		$localBranches = getLocalBranches( $project );
		
		foreach( $localBranches as $localBranch ){
			if( !in_array( $localBranch, $ignored ) ){
				deleteBranches($localBranch);
			}
		}
	}
	
	/**
	 * check if the current branch is up to date with master
	 * @param string $project
	 * @return multitype:
	 */
	function checkBranches( $project="UniteZOne" )
	{
		$output = sanitizeWorkingBranch( $project );
		
		if( empty( $output )){
			$output = runGit("rev-list HEAD..origin", $project);
			if( !empty( $output ) ){
				echo "<br/><h2> Branch is behind master by ".count($output)." , contact programmer to update </h2><br/>";				
			}
		}else{
			print_r($output);
			echo "<br/><h2> Branch is not clean: contact Administrator </h2><br/>";
		}
		
		return $output;
		
	}
	
	/**
	 * Check if the current branch is free from conflicts
	 * @param string $project
	 */
	function sanitizeWorkingBranch( $project="UniteZOne" )
	{
		$output = runGit("status --porcelain -uno", $project);
		return $output;
	}
	
	/**
	 * delete the branch  provided
	 * @param string $branchName
	 * @param string $project
	 * @return array of output lines:
	 */
	function deleteBranches( $branchName, $project="UniteZOne")
	{
		$output = runGit( "branch -D $branchName", $project);
		return $output;
				
	}
	
	/**
	 * Update current working branch
	 * @param string $branchName
	 * @param string $project
	 * @return array of output lines
	 */
	function pullCurrentBranch( $branchName, $project="UniteZOne")
	{
		$output = runGit( "pull origin $branchName", $project);
		return $output;
	}



	/**
	 * Executes a git command in the project repo.
	 * @param $project string Project name
	 * @param $command string Command to run
	 * @return array of output lines
	 */
	function runGit($command, $project="UniteZOne")
	{
		global $conf;
	
		$output = array();
		$cmd = $conf['git'] ." --git-dir=". escapeshellarg($conf['projects'][$project]['repo'])."/.git" ." --work-tree=". escapeshellarg($conf['projects'][$project]['repo']) ." $command";
		$ret = 0;
		exec($cmd, $output, $ret);
		//if ($ret != 0) { die('FATAL: exec() for git failed, is the path properly configured?'); }
		return $output;
	}
	
	/**
	 * Executes a git command in the project repo, sending output directly to the
	 * client.
	 * @param $project string Project name
	 * @param $command string Command to run
	 * @return array of output lines
	 */
	function runGitPassthru($command, $project="UniteZOne")
	{
		global $conf;
	
		$cmd = $conf['git'] ." --git-dir=". escapeshellarg($conf['projects'][$project]['repo'])."/.git" ." --work-tree=". escapeshellarg($conf['projects'][$project]['repo']) ." $command";
		$result = 0;
		passthru($cmd, $result);
		return $result;
	}

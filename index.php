<?php

	/** @file
	 * The main "controller" file of git control.
	 *
	 */
	
	require_once('conf/config.php');
	require_once('inc/functions.inc');
	
	$project = $_REQUEST['project'] ?: "UniteZone";
	
	// Adjust error_reporting based on config.
	if ($conf['debug']) {
		error_reporting(E_ALL ^ E_NOTICE);
	}
	
	if ( !in_array( $_SERVER['HOSTNAME'], $conf['servers'] ) ){
		die( " This script cannot be run on this server ");
	}
	
	if( $_REQUEST['action'] == "switchBranch" && !empty( $_REQUEST['branch_name'] ) ){
		switchBranch( $_REQUEST['branch_name'], $project );
	}
	
	if( $_REQUEST['action'] == "refreshBranch" ){
		refreshRemoteBranches( $project );
	}
	
	if( $_REQUEST['action'] == "pullCurrentBranch" && !empty( $_REQUEST['currentBranch'] ) ){
		pullCurrentBranch( $_REQUEST['currentBranch'], $project );
	}
	
	
	
	$remoteBranches = getRemoteBranches( $project );
	
	$currentBranch = getCurrentBranch( $project );
	
	sanitizeWorkingBranch( $project );
	
	echo "<br/> Current Branch : ". $currentBranch ."<br/>";
	
	
	?>
	<table>
       	<thead>
       		<tr class="row">
	            <th width="300"><div align="left">Choose Branch</div></th>
      		</tr>
      	</thead>
      	
      	
        <tbody>
        	 <tr>
        	 <form action="<?php echo $PHP_SELF;?>" method="post" > 
        	 <input type="hidden" value="switchBranch" name="action">
				<td>
					<select name='branch_name' id='branch_name' style="width:200px" >
					<?php
						foreach( $remoteBranches as $remoteBranch){
							if( ( $remoteBranch !== $currentBranch )  && ( strpos($remoteBranch, "HEAD") === FALSE)){
								echo  "<option value='".$remoteBranch."'>".$remoteBranch."</option>";
							}
						}            
						?>
					</select>
				</td>	
				<td>	<input type="submit" value="switch" > </td>
				</form>
			</tr>
			<tr></tr>
			<tr>
				<td>
					<form action="<?php echo $PHP_SELF;?>" method="post" > 
						<input type="hidden" value="refreshBranch" name="action">
						<input type="submit" value="Refresh Remote Branches" > 
					</form>
				</td>
			</tr>
			<tr>
				<td>
					<form action="<?php echo $PHP_SELF;?>" method="post" > 
						<input type="hidden" value="pullCurrentBranch" name="action">
						<input type="hidden" value="<?php echo $currentBranch; ?>" name="currentBranch">
						<input type="submit" value="Update Current Branch" > 
					</form>
				</td>
			</tr>
			
		</tbody>
		<tr>
			
		</tr>
	</table>
	
	

